#include "name_resolver.h"

#include <memory>
#include <string>

#include "../compilation_pass.h"
#include "../entities/entity.h"
/*
* note:
* do NOT include the entity headers directly
* they will get transitively included by identifier_expression.h
* and will throw an error. Really identifier expression should have
* a cpp file and implement it there, but thats overcomplicated
* so for now im leaving it like this.
*/
#include "../symbolics/symbol_table.h"
#include "../types/types_fwd.h"
#include "../types/custom_type.h"
#include "../types/container_types.h"
#include "../ast/declarations/function_declaration.h"
#include "../ast/declarations/module_declaration.h"
#include "../ast/declarations/type_declaration.h"
#include "../ast/declarations/variable_declaration.h"
#include "../ast/expressions/binary_expression.h"
#include "../ast/expressions/function_call.h"
#include "../ast/expressions/identifier_expression.h"
#include "../ast/expressions/member_expression.h"
#include "../ast/expressions/this_expression.h"
#include "../ast/statements/block_statement.h"
#include "../ast/statements/control_flow/for_loop.h"
#include "../ast/statements/control_flow/if_statement.h"
#include "../ast/statements/control_flow/while_loop.h"
#include "../../error/error_catalog.h"
#include "../../utils/string_utils.h"

void name_resolver::visit(std::shared_ptr<function_declaration> func) {
	if (func->body) {
		sym_table->push_scope();

		if (func->is_constructor) {
			current_ctor = func;
		}
		else {
			current_func = func;
		}

		// add generics to the current scope
		for (const auto& generic : func->generics) {
			auto err = sym_table->add_generic(generic);
			if (err == ERR_DUPLICATE_GENERIC) {
				ERROR(ERR_DUPLICATE_GENERIC, func->position, generic->identifier.c_str());
				continue;
			}
			else if (err == ERR_GENERIC_SHADOWS_VARIABLE) {
				// we know they have the same name with this error so we can use this sneaky trick to name both
				ERROR(ERR_GENERIC_SHADOWS_VARIABLE, func->position, generic->identifier.c_str(), generic->identifier.c_str());
				continue;
			}
		}

		// add parameters to the current scope
		for (const auto& param : func->parameters) {
			if (sym_table->has_variable(param->identifier)) {
				ERROR(ERR_DUPLICATE_PARAMETER, func->position, param->identifier.c_str());
				continue;
			}
			if (sym_table->add_variable(param) == ERR_VARIABLE_SHADOWS_GENERIC) {
				// we know they have the same name with this error so we can use this sneaky trick to name both
				ERROR(ERR_VARIABLE_SHADOWS_GENERIC, func->position, param->identifier.c_str(), param->identifier.c_str());
				continue;
			}
		}

		func->body->accept(*this);
		current_func = nullptr;
		current_ctor = nullptr;
		sym_table->pop_scope();
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
		auto err = sym_table->add_variable(var);
		if (err == ERR_VARIABLE_ALREADY_DECLARED_SCOPE) {
			ERROR(ERR_VARIABLE_ALREADY_DECLARED_SCOPE, var->position, var->identifier.c_str());
			return;
		}
		else if (err == ERR_VARIABLE_SHADOWS_GENERIC) {
			// we know they have the same name with this error so we can use this sneaky trick to name both
			ERROR(ERR_VARIABLE_SHADOWS_GENERIC, var->position, var->identifier.c_str(), var->identifier.c_str());
			return;
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
void name_resolver::visit(std::shared_ptr<module_declaration> decl) {
	auto old_symbols = sym_table;
	resolver.current_module = decl->module_info;
	sym_table = &decl->module_info->symbols;
	for (const auto& decl : decl->declarations) {
		decl->accept(*this);
	}
	resolver.current_module = decl->module_info->parent_module;
	sym_table = old_symbols;
}
void name_resolver::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}
void name_resolver::visit(std::shared_ptr<identifier_expression> expr) {
	// variable identifier
	auto var_decl = resolver.get_variable(current_type, expr->identifier);
	if (var_decl.ambiguous()) {
		auto names = var_decl.results_names();
		std::string names_str = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, expr->position, expr->identifier.c_str(), names_str.c_str());
		return;
	}
	else if (var_decl.found()) {
		expr->id_type = IDENTIFIER_VARIABLE;
		expr->variable_declaration = var_decl.first()->as_variable()->declaration;
		return;
	}

	// module identifier
	auto mod_decl = module_manager.get_module(expr->identifier);
	if (mod_decl != nullptr) {
		expr->id_type = IDENTIFIER_MODULE;
		expr->module_info = mod_decl;
		return;
	}

	// function identifier
	/*auto func_decl = resolver.get_function(expr->identifier);
	if (var_decl.error == LOOKUP_OK) {
		expr->id_type = IDENTIFIER_VARIABLE;
		expr->function_declaration = std::get<std::shared_ptr<function_declaration>>(var_decl.value);
		return;
	} WE DONT SUPPORT METHOD POINTERS ETC CURRENTLY SO THIS IS JUST COMMENTED */

	// type identifier
	auto type_decl = resolver.get_type(expr->identifier);
	if (type_decl.ambiguous()) {
		auto names = type_decl.results_names();
		std::string names_str = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, expr->position, expr->identifier.c_str(), names_str.c_str());
		return;
	}
	else if (type_decl.found()) {
		expr->id_type = IDENTIFIER_TYPE;
		expr->type_declaration = type_decl.first()->as_type()->type->as_custom()->declaration;
		return;
	}
	// TODO: support for errors like collisions im just too lazy to implement them currently

	// enum identifier
	auto enum_decl = resolver.get_enum(expr->identifier);
	if (enum_decl.ambiguous()) {
		auto names = enum_decl.results_names();
		std::string names_str = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, expr->position, expr->identifier.c_str(), names_str.c_str());
		return;
	}
	else if (enum_decl.found()) {
		expr->id_type = IDENTIFIER_ENUM;
		expr->enum_declaration = enum_decl.first()->as_type()->type->as_enum()->declaration;
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
			// error
		}

		// module
		if (entity->kind() == ENTITY_MODULE) {
			const auto& mod_info = entity->as_module()->mod_info;
			const auto& syms = mod_info->symbols;

			auto result = syms.lookup(expr->member);
			if (result.ambiguous()) {
				auto names = result.results_names();
				std::string names_str = string_utils::vec_to_string(names);
				ERROR(ERR_NAME_COLLISION, expr->position, expr->member.c_str(), names_str.c_str());
				return;
			}
			else if (result.found()) {
				expr->resolved_entity = result.first();
			}
			else {
				ERROR(ERR_NO_MEMBER_WITH_NAME_MODULE, expr->position, mod_info->full_name(), expr->member.c_str());
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
					expr->resolved_entity = variable_entity::make(field);
				}
				else {
					ERROR(ERR_STATIC_ACCESS_ON_NONSTATIC_MEMBER, expr->position, expr->member.c_str());
					return;
				}
			}
		}
		// function
		else if (entity->kind() == ENTITY_FUNCTION) {
			ERROR(ERR_STATIC_ACCESS_ON_FUNCTION, expr->position, entity->name().c_str());
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

		// module::function()
		if (entity->kind() == ENTITY_MODULE) {
			auto syms = entity->as_module()->mod_info->symbols;

			auto candidates = syms.get_function_candidates(func_call->identifier, func_call->args.size(), func_call->generic_args.size());
			func_call->declaration_candidates = candidates;
		}
		// type::function()
		else if (entity->kind() == ENTITY_TYPE) {
			// lookup static method
			auto custom = entity->as_type()->type->as_custom();
			if (!custom) {
				ERROR(ERR_METHOD_ACCESS_ON_NONCOMPOSITE, func_call->position, entity->name().c_str());
				return;
			}

			// TODO:
			// - gather candidates
			// - ensure candidates are static
		}
		// function::function()
		else if (entity->kind() == ENTITY_FUNCTION) {
			ERROR(ERR_STATIC_ACCESS_ON_FUNCTION, func_call->position, entity->name().c_str());
			return;
		}
	}
	else if (func_call->is_method()) {
		// only try to resolve the caller_obj, not the method
		func_call->caller_obj->accept(*this);
	}
	else {
		// check if its a constructor call
		auto result = resolver.get_type(func_call->identifier);
		if (result.ambiguous()) {
			func_call->is_constructor = true;
			auto names = result.results_names();
			std::string names_str = string_utils::vec_to_string(names);
			ERROR(ERR_NAME_COLLISION, func_call->position, func_call->identifier.c_str(), names_str.c_str());
			return;
		}
		else if (result.found()) {
			// DONT RESOLVE CANDIDATES YET - GENERIC CONSTRUCTORS HAVNT BEEN INSTANTIATED
			// we can still set the constructor type with the generic definition for later use
			func_call->is_constructor = true;
			func_call->ctor_type = result.first()->as_type()->type->as_custom()->declaration;
		}
		else {
			// lookup function as normal
			auto func_candidates = resolver.get_function_candidates(func_call->identifier, func_call->args.size(), func_call->generic_args.size());
			func_call->declaration_candidates = func_candidates;
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
