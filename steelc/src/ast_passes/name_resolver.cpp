#include "name_resolver.h"

#include <memory>
#include <string>

#include <ast/ast.h>
#include <compiler/compilation_pass.h>
#include <modules/module_manager.h>
#include <symbolics/lookup_result.h>
#include <symbolics/symbol_table.h>
#include <symbolics/symbol_error.h>
#include <representations/entities/entity.h>
#include <representations/entities/variable_entity.h>
#include <representations/entities/function_entity.h>
#include <representations/entities/type_entity.h>
#include <representations/entities/module_entity.h>
#include <representations/types/types_fwd.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <ast_passes/helpers/function_filter.h>
#include <error/error_catalog.h>
#include <utils/string_utils.h>

void name_resolver::visit(std::shared_ptr<function_declaration> func) {
	if (func->body) {
		sym_table->push_scope();
		sym_table->push_generic_scope();

		if (func->is_constructor) {
			current_ctor = func;
		}
		else {
			current_func = func;
		}

		// add generics to the current scope
		for (const auto& generic : func->generics) {
			symbol_error err = sym_table->add_symbol(generic);
			if (err != SYMBOL_OK) {
				switch (err) {
				case SYMBOL_CONFLICTS_WITH_GENERIC:
					ERROR(ERR_DUPLICATE_GENERIC, func->position, generic->identifier.c_str());
					continue;
				case SYMBOL_CONFLICTS_WITH_VARIABLE:
					// we know they have the same name with this error so we can use this sneaky trick to name both
					ERROR(ERR_GENERIC_SHADOWS_VARIABLE, func->position, generic->identifier.c_str(), generic->identifier.c_str());
					continue;
				default:
					ERROR(ERR_INTERNAL_ERROR, func->position, "Name Resolver", "Unknown error while adding generic parameter");
					continue;
				}
			}
		}

		// add parameters to the current scope
		for (const auto& param : func->parameters) {
			symbol_error err = sym_table->add_symbol(param);
			if (err != SYMBOL_OK) {
				switch (err) {
				case SYMBOL_CONFLICTS_WITH_GENERIC:
					ERROR(ERR_VARIABLE_SHADOWS_GENERIC, func->position, param->identifier.c_str(), param->identifier.c_str());
					continue;
				case SYMBOL_CONFLICTS_WITH_VARIABLE:
					// only possible if the conflicting variable is another parameter
					// so we know its this error
					ERROR(ERR_DUPLICATE_PARAMETER, func->position, param->identifier.c_str());
					continue;
				case SYMBOL_CONFLICTS_WITH_FUNCTION:
					// im not sure if this is possible, but we'll add it just in case
					ERROR(ERR_NAME_CONFLICT, func->position, "parameter", param->identifier.c_str(), "function");
					continue;
				// we dont need to check other cases as they are impossible since parameters will always be scoped
				default:
					ERROR(ERR_INTERNAL_ERROR, func->position, "Name Resolver", "Unknown error while adding parameter");
					continue;
				}
			}
		}

		func->body->accept(*this);
		current_func = nullptr;
		current_ctor = nullptr;
		sym_table->pop_scope();
		sym_table->pop_generic_scope();
	}

	resolve_type_names(func->return_type);
	for (const auto& param : func->parameters) {
		resolve_type_names(param->type);
	}
}
void name_resolver::visit(std::shared_ptr<variable_declaration> var) {
	// only check local variables here - globals are already handled
	// in the declaration collector pass
	if (!sym_table->in_global_scope()) {
		symbol_error err = sym_table->add_symbol(var, current_type_entity());
		// only check for scoped errors
		// module level errors wont occur here
		// as were in a local scope
		if (err != SYMBOL_OK) {
			switch (err) {
			case SYMBOL_CONFLICTS_WITH_VARIABLE:
				ERROR(ERR_VARIABLE_ALREADY_DECLARED_SCOPE, var->position, var->identifier.c_str());
				break;
			case SYMBOL_CONFLICTS_WITH_GENERIC:
				// we know both have same name so use same string
				ERROR(ERR_VARIABLE_SHADOWS_GENERIC, var->position, var->identifier.c_str(), var->identifier.c_str());
				break;

			default:
				ERROR(ERR_INTERNAL_ERROR, var->position, "Declaration Collector", "Unknown error while adding variable symbol");
				break;
			}
		}
	}
	// still accept all initializers including global ones
	if (var->initializer) {
		var->initializer->accept(*this);
	}

	resolve_type_names(var->type);
}
void name_resolver::visit(std::shared_ptr<type_declaration> decl) {
	sym_table->push_scope();
	current_type = decl;
	for (const auto& constructor : decl->constructors) {
		constructor->accept(*this);
	}
	for (const auto& member : decl->fields) {
		member->accept(*this);
	}
	for (const auto& method : decl->methods) {
		method->accept(*this);
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}
	current_type = nullptr;
	sym_table->pop_scope();
}
void name_resolver::visit(std::shared_ptr<module_declaration> module) {
	auto old_symbols = sym_table;
	resolver.current_module = module->entity;
	sym_table = &module->entity->symbols();
	for (const auto& decl : module->declarations) {
		decl->accept(*this);
	}
	resolver.current_module = module->entity->parent_module;
	sym_table = old_symbols;
}
void name_resolver::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}
void name_resolver::visit(std::shared_ptr<identifier_expression> expr) {
	// resolve identifier
	auto entity = resolver.lookup(expr->identifier, current_type_entity());

	if (entity.ambiguous()) {
		auto names = entity.results_names();
		std::string names_str = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, expr->position, expr->identifier.c_str(), names_str.c_str());
		return;
	}
	else if (entity.found()) {
		expr->entity_ref = entity.first()->ref();
		return;
	}

	ERROR(ERR_UNKNOWN_IDENTIFIER, expr->position, expr->identifier.c_str());
}
void name_resolver::visit(std::shared_ptr<member_expression> expr) {
	// we CAN resolve member expressions here but ONLY static ones
	// non-static member expressions are resolved in the type checker pass
	expr->object->accept(*this);

	if (expr->is_static_access()) {
		auto entity = expr->object->entity();
		if (!entity) {
			ERROR(ERR_STATIC_ACCESS_INVALID, expr->position, entity->kind_string().c_str(), entity->name().c_str());
			return;
		}
		else if (entity == entity::UNRESOLVED) {
			// dont bother reporting an error here
			return;
		}

		// module
		if (entity->kind() == ENTITY_MODULE) {
			const auto& mod = entity->as_module();
			const auto& syms = mod->symbols();

			auto result = syms.lookup(expr->member);
			if (result.ambiguous()) {
				auto names = result.results_names();
				std::string names_str = string_utils::vec_to_string(names);
				ERROR(ERR_NAME_COLLISION, expr->position, expr->member.c_str(), names_str.c_str());
				return;
			}
			else if (result.found()) {
				expr->resolved_entity = result.first()->ref();
			}
			else {
				ERROR(ERR_NO_MEMBER_WITH_NAME_MODULE, expr->position, mod->full_name(), expr->member.c_str());
				return;
			}
		}
		// type
		else if (entity->kind() == ENTITY_TYPE) {
			// lookup static member
			auto custom = entity->as_type()->type->as_custom();
			if (!custom) {
				ERROR(ERR_MEMBER_ACCESS_ON_NONCOMPOSITE, expr->position, entity->name().c_str());
				return;
			}

			for (const auto& field : custom->declaration->fields) {
				if (field->identifier != expr->member) {
					continue;
				}

				if (false) {
					//expr->entity_ref = variable_entity::make(field);
				}
				else {
					ERROR(ERR_STATIC_ACCESS_NONSTATIC_MEMBER, expr->position, expr->member.c_str());
					return;
				}
			}
		}
		// other
		else {
			ERROR(ERR_STATIC_ACCESS_INVALID, expr->position, entity->kind_string().c_str(), entity->name().c_str());
			return;
		}
	}
}
void name_resolver::visit(std::shared_ptr<this_expression> expr) {
	if (!current_type && !current_func && !current_ctor) {
		ERROR(ERR_THIS_OUTSIDE_NONSTATIC_METHOD, expr->position);
		return;
	}
	expr->parent_type = current_type->type();
}
void name_resolver::visit(std::shared_ptr<function_call> func_call) {
	if (func_call->is_scoped_function()) {
		// resolve scope (statically)
		func_call->scope->accept(*this);

		// use entity instead of type for static access
		auto entity = func_call->scope->entity();

		// ast node doesnt refer to an entity
		if (!entity) {
			ERROR(ERR_CALL_ON_NON_FUNCTION, func_call->position, func_call->identifier.c_str());
			return;
		}
		// ast node does refer to an entity but its unresolved
		else if (entity == entity::UNRESOLVED) {
			// dont bother reporting an error here
			return;
		}

		switch (entity->kind()) {
		case ENTITY_VARIABLE: {
			ERROR(ERR_STATIC_ACCESS_INVALID, func_call->position, entity->kind_string().c_str(), entity->name().c_str());
			return;
		}
		// function::function()
		case ENTITY_FUNCTION: {
			ERROR(ERR_STATIC_ACCESS_INVALID, func_call->position, entity->kind_string().c_str(), entity->name().c_str());
			return;
		}
		// type::function()
		case ENTITY_TYPE: {
			// lookup static method
			auto custom = entity->as_type()->type->as_custom();
			if (!custom) {
				ERROR(ERR_METHOD_ACCESS_ON_NONCOMPOSITE, func_call->position, entity->name().c_str());
				return;
			}

			// TODO:
			// - gather candidates
			// - ensure candidates are static
			return;
		}
		// module::function()
		case ENTITY_MODULE: {
			const auto& symbols = entity->as_module()->symbols();
			auto result = symbols.lookup(func_call->identifier);

			if (result.no_match()) {
				ERROR(ERR_UNKNOWN_FUNCTION, func_call->position, func_call->identifier.c_str());
				return;
			}

			std::vector<std::shared_ptr<function_entity>> func_entities;
			for (const auto& res : result.results) {
				if (res->kind() == ENTITY_FUNCTION) {
					func_entities.push_back(res->as_function());
				}
				else {
					// might change this in the future, e.g. for function pointer variables
					ERROR(ERR_CALL_ON_NON_FUNCTION, func_call->position, func_call->identifier.c_str());
					return;
				}
			}

			function_filter filter(func_entities);
			auto candidates = filter.filter_by_param_count(
				func_call->args.size(),
				func_call->generic_args.size()
			);

			for (const auto& candidate : candidates) {
				func_call->declaration_candidates.push_back(candidate->declaration);
			}
			return;
		}
		// we might need to support other cases but im not sure
		}
	}
	else if (func_call->is_method()) {
		// only try to resolve the callee, not the method
		func_call->callee->accept(*this);
	}
	else if (func_call->identifier == "printf") {
		// temporary - dont resolve printf calls
	}
	else {
		// check if its a constructor call
		lookup_result type = resolver.lookup(func_call->identifier);
		if (type.ambiguous()) {
			auto names = type.results_names();
			std::string names_str = string_utils::vec_to_string(names);
			ERROR(ERR_NAME_COLLISION, func_call->position, func_call->identifier.c_str(), names_str.c_str());
			return;
		}
		else if (type.found() && type.first()->kind() == ENTITY_TYPE) {
			// DONT RESOLVE CANDIDATES YET - GENERIC CONSTRUCTORS HAVNT BEEN INSTANTIATED
			// we can still set the constructor type with the generic definition for later use
			func_call->is_constructor = true;
			func_call->ctor_type = type.first()->as_type()->type->as_custom()->declaration;
			return;
		}
		
		// lookup function as normal
		lookup_result result = resolver.lookup(func_call->identifier);

		if (result.no_match()) {
			ERROR(ERR_UNKNOWN_FUNCTION, func_call->position, func_call->identifier.c_str());
			return;
		}

		std::vector<std::shared_ptr<function_entity>> func_entities;
		for (const auto& res : result.results) {
			if (res->kind() == ENTITY_FUNCTION) {
				func_entities.push_back(res->as_function());
			}
			else {
				// might change this in the future, e.g. for function pointer variables
				ERROR(ERR_CALL_ON_NON_FUNCTION, func_call->position, func_call->identifier.c_str());
				return;
			}
		}

		function_filter filter(func_entities);
		auto candidates = filter.filter_by_param_count(
			func_call->args.size(),
			func_call->generic_args.size()
		);

		for (const auto& candidate : candidates) {
			func_call->declaration_candidates.push_back(candidate->declaration);
		}
	}

	// resolve args as normal
	for (auto& arg : func_call->args) {
		arg->accept(*this);
	}
}
void name_resolver::visit(std::shared_ptr<code_block> block) {
	if (!block->is_body) {
		sym_table->push_scope();
	}
	for (const auto& stmt : block->body) {
		stmt->accept(*this);
	}
	if (!block->is_body) {
		sym_table->pop_scope();
	}
}
void name_resolver::visit(std::shared_ptr<if_statement> if_stmt) {
	// may change this later to support inline variable if statements
	if_stmt->condition->accept(*this);
	//sym_table->push_scope();
	if_stmt->then_block->accept(*this);
	//sym_table->pop_scope();
	if (if_stmt->else_node) {
		if_stmt->else_node->accept(*this);
	}
}
void name_resolver::visit(std::shared_ptr<for_loop> for_loop) {
	sym_table->push_scope();
	if (for_loop->initializer) {
		for_loop->initializer->accept(*this);
	}
	if (for_loop->condition) {
		for_loop->condition->accept(*this);
	}
	if (for_loop->increment) {
		for_loop->increment->accept(*this);
	}
	for_loop->body->accept(*this);
	sym_table->pop_scope();
}
void name_resolver::visit(std::shared_ptr<while_loop> while_loop) {
	if (while_loop->condition) {
		while_loop->condition->accept(*this);
	}
	sym_table->push_scope();
	while_loop->body->accept(*this);
	sym_table->pop_scope();
}

std::shared_ptr<type_entity> name_resolver::current_type_entity() const {
	if (current_type) {
		return current_type->entity();
	}
	return nullptr;
}

void name_resolver::resolve_type_names(type_ptr& type) {
	if (auto ptr = type->as_pointer()) {
		resolve_type_names(ptr->base_type);
	}
	else if (auto arr = type->as_array()) {
		resolve_type_names(arr->base_type);

		if (arr->size_expression) {
			arr->size_expression->accept(*this);
		}
	}
}
