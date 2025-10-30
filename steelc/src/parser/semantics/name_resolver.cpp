#include "name_resolver.h"

#include <variant>
#include <memory>

#include "../ast/ast.h"
#include "../types/custom_type.h"

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
	if (var_decl.error == LOOKUP_OK) {
		expr->id_type = IDENTIFIER_VARIABLE;
		expr->variable_declaration = std::get<std::shared_ptr<variable_declaration>>(var_decl.value);
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
	if (type_decl.error == LOOKUP_OK) {
		expr->id_type = IDENTIFIER_TYPE;
		expr->type_declaration = std::get<std::shared_ptr<type_declaration>>(type_decl.value);
		return;
	}
	// TODO: support for errors like collisions im just too lazy to implement them currently

	// enum identifier
	auto enum_decl = resolver.get_enum(expr->identifier);
	if (enum_decl.error == LOOKUP_OK) {
		expr->id_type = IDENTIFIER_ENUM;
		expr->enum_declaration = std::get<std::shared_ptr<enum_declaration>>(enum_decl.value);
		return;
	}

	ERROR(ERR_UNKNOWN_IDENTIFIER, expr->position, expr->identifier.c_str());
}
void name_resolver::visit(std::shared_ptr<this_expression> expr) {
	if (!current_type && !current_func && !current_ctor) {
		ERROR(ERR_THIS_OUTSIDE_NONSTATIC_METHOD, expr->position);
		return;
	}
	expr->parent_type = current_type->type();
}
void name_resolver::visit(std::shared_ptr<function_call> func_call) {
	if (func_call->is_method()) {
		// only try to resolve the callee, not the method
		func_call->callee->accept(*this);
	}
	else {
		// check if its a constructor call
		auto result = resolver.get_type(func_call->identifier);
		if (result.error == LOOKUP_OK) {
			// DONT RESOLVE CANDIDATES YET - GENERIC CONSTRUCTORS HAVNT BEEN INSTANTIATED
			// we can still set the constructor type with the generic definition for later use
			func_call->ctor_type = std::get<std::shared_ptr<type_declaration>>(result.value);
		}
		else if (result.error == LOOKUP_COLLISION) {
			ERROR(ERR_NAME_COLLISION, func_call->position, func_call->identifier.c_str());
			// we should still set the call to be a constructor call but i dont know how to do
			// that yet
			return;
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