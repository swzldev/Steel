#include "type_resolver.h"

#include "../ast/ast.h"
#include "../parser_utils.h"
#include "../types/types.h"
#include "../types/custom_types.h"
#include "../types/container_types.h"

void type_resolver::visit(std::shared_ptr<function_declaration> func) {
	sym_table->push_scope();
	// add generics to the current scope
	for (const auto& generic : func->generics) {
		auto err = sym_table->add_generic(generic);
		if (err == ERR_DUPLICATE_GENERIC) {
			ERROR(ERR_DUPLICATE_GENERIC, func->position, generic->identifier.c_str());
			continue;
		}
	}

	// resolve return type
	if (!func->is_constructor) {
		// (if its a constructor we already know its return type)
		resolve_type(func->return_type, func);
	}

	// resolve parameter types
	for (auto& param : func->parameters) {
		resolve_type(param->type, param);
	}

	if (func->body) {
		func->body->accept(*this);
	}
	sym_table->pop_scope();
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

	// resolve base types
	for (type_ptr& base : decl->base_types) {
		resolve_type(base, decl);
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
	for (auto& gen_arg : func_call->generic_args) {
		resolve_type(gen_arg, func_call);
	}
	for (auto& arg : func_call->args) {
		arg->accept(*this);
	}
}

void type_resolver::resolve_type(type_ptr& type, ast_node_ptr resolvee) {
	if (auto custom = std::dynamic_pointer_cast<custom_type>(type)) {
		std::string type_name = custom->name();
		// lookup generic
		auto generic = sym_table->get_generic(type_name);
		if (generic.error == LOOKUP_OK) {
			auto gn = std::make_shared<generic_type>(type_name);
			gn->declaration = std::get<std::shared_ptr<generic_parameter>>(generic.value);
			type = gn;
			return;
		}
		// lookup type
		auto decl = resolver.get_type(type_name);
		if (decl.error != LOOKUP_OK) {
			if (decl.error == LOOKUP_COLLISION) {
				ERROR(ERR_NAME_COLLISION, resolvee->position, type_name.c_str());
				return;
			}
			ERROR(ERR_UNKNOWN_TYPE, resolvee->position, type_name.c_str());
			return;
		}
		custom->declaration = std::get<std::shared_ptr<type_declaration>>(decl.value);
	}
}