#include "type_resolver.h"

#include "../ast/ast.h"
#include "../parser_utils.h"
#include "../types/types.h"
#include "../types/custom_types.h"

void type_resolver::visit(std::shared_ptr<function_declaration> func) {
	// resolve return type
	resolve_type(func->return_type, func);

	// resolve parameter types
	for (auto& param : func->parameters) {
		resolve_type(param->type, param);
	}

	func->body->accept(*this);
}
void type_resolver::visit(std::shared_ptr<constructor_declaration> constructor) {
	// resolve parameter types
	for (auto& param : constructor->parameters) {
		resolve_type(param->type, param);
	}

	constructor->body->accept(*this);
}
void type_resolver::visit(std::shared_ptr<variable_declaration> var) {
	// resolve variable type
	resolve_type(var->type, var);
}
void type_resolver::visit(std::shared_ptr<type_declaration> decl) {
	// resolve member types
	for (auto& member : decl->fields) {
		resolve_type(member->type, member);
	}
	// resolve constructor types
	for (auto& constructor : decl->constructors) {
		constructor->accept(*this);
	}
	// resolve method types
	for (auto& method : decl->methods) {
		method->accept(*this);
	}
	// resolve operator types
	for (auto& op : decl->operators) {
		op->accept(*this);
	}
}
void type_resolver::visit(std::shared_ptr<module_declaration> decl) {
	resolver.current_module = decl->module_info;
	for (auto& decl : decl->declarations) {
		decl->accept(*this);
	}
	resolver.current_module = decl->module_info->parent_module;
}
void type_resolver::visit(std::shared_ptr<function_call> func_call) {
	for (auto& arg : func_call->args) {
		arg->accept(*this);
	}
}
void type_resolver::visit(std::shared_ptr<constructor_call> constructor_call) {
	for (auto& arg : constructor_call->args) {
		arg->accept(*this);
	}
}

void type_resolver::resolve_type(type_ptr& type, ast_ptr resolvee) {
	// this function seems to lack any errors
	// the reason is because no custom types will break through
	// that dont exist, as they will be caught as an "unknown identifier"
	// in the parser
	if (auto custom = std::dynamic_pointer_cast<custom_type>(type)) {
		auto decl = resolver.get_type(custom->identifier);
		if (decl.error != LOOKUP_OK) {
			if (decl.error == LOOKUP_COLLISION) {
				ERROR(ERR_NAME_COLLISION, resolvee->position, custom->identifier.c_str());
				return;
			}
			ERROR(ERR_INTERNAL_ERROR, resolvee->position, "Type Resolver", "Attempted to resolve a type that didn't exist");
			return;
		}
		custom->declaration = std::get<std::shared_ptr<type_declaration>>(decl.value);
	}
}