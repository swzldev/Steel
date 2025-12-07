#include "type_resolver.h"

#include "../ast/ast.h"
#include "../types/data_type.h"
#include "../types/custom_type.h"
#include "../types/container_types.h"
#include "../../utils/string_utils.h"

void type_resolver::visit(std::shared_ptr<function_declaration> func) {
	sym_table->push_scope();
	// add generics to the current scope
	for (const auto& generic : func->generics) {
		generic->param_index = cur_generic_index++;
		auto err = sym_table->add_generic(generic);
		if (err == ERR_DUPLICATE_GENERIC) {
			ERROR(ERR_DUPLICATE_GENERIC, func->position, generic->identifier.c_str());
			continue;
		}
	}

	// resolve return type
	if (!func->is_constructor) {
		// (if its a constructor we already know its return type)
		resolve_type(func->return_type);
	}

	// resolve parameter types
	for (auto& param : func->parameters) {
		resolve_type(param->type);
	}

	if (func->body) {
		func->body->accept(*this);
	}
	sym_table->pop_scope();
}
void type_resolver::visit(std::shared_ptr<variable_declaration> var) {
	// resolve variable type
	resolve_type(var->type);
}
void type_resolver::visit(std::shared_ptr<type_declaration> decl) {
	sym_table->push_scope();
	// add generics to the current scope
	for (const auto& generic : decl->generics) {
		generic->param_index = cur_generic_index++;
		auto err = sym_table->add_generic(generic);
		if (err == ERR_DUPLICATE_GENERIC) {
			ERROR(ERR_DUPLICATE_GENERIC, decl->position, generic->identifier.c_str());
			continue;
		}
	}

	// resolve member types
	for (auto& member : decl->fields) {
		resolve_type(member->type);
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
		resolve_type(base);
	}
	sym_table->pop_scope();
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
		resolve_type(gen_arg);
	}
	for (auto& arg : func_call->args) {
		arg->accept(*this);
	}
}

void type_resolver::resolve_type(type_ptr& type) {
	// resolve pointer types
	if (type->is_pointer()) {
		auto ptr = type->as_pointer();
		if (ptr && ptr->base_type) {
			resolve_type(ptr->base_type);
		}
		return;
	}
	// resolve array types
	if (type->is_array()) {
		auto arr = type->as_array();
		if (arr && arr->base_type) {
			resolve_type(arr->base_type);
		}
		return;
	}

	// resolve custom types and generics to their declarations
	if (type->is_custom()) {
		resolve_custom(type);
	}

	// resolve & check generic args (if applicable)
	if (type->generic_args.size() > 0) {
		if (!type->is_custom()) {
			ERROR(ERR_GENERIC_ARGS_ON_NONCUSTOM_TYPE, type->position, type->name().c_str());
			return;
		}

		auto custom = type->as_custom();
		if (custom->declaration) {
			if (!custom->declaration->is_generic) {
				ERROR(ERR_GENERIC_ARGS_ON_NONGENERIC_TYPE, type->position, type->name().c_str());
				return;
			}

			if (custom->declaration->generics.size() != type->generic_args.size()) {
				std::string args;
				for (auto& arg : custom->declaration->generics) {
					if (!args.empty()) args += ", ";
					args += arg->identifier;
				}
				ERROR(ERR_INCORRECT_NUMBER_OF_GENERIC_ARGS, type->position, type->name().c_str(), args.c_str());
				return;
			}
		}

		// resolve all args
		for (auto& gen_arg : type->generic_args) {
			resolve_type(gen_arg);
		}
	}
}

void type_resolver::resolve_custom(type_ptr& custom) {
	const std::string& type_name = custom->name();

	// generic type
	auto generic = sym_table->get_generic(type_name);
	if (generic) {
		auto gn = std::make_shared<generic_type>(type_name);
		gn->generic_param_index = generic->param_index;
		custom = gn;
		return;
	}

	// custom type
	auto decl = resolver.get_type(type_name);
	if (decl.ambiguous()) {
		auto names = decl.results_names(true);
		std::string names_string = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, custom->position, type_name.c_str(), names_string.c_str());
		return;
	}
	else if (decl.found()) {
		custom = decl.first()->as_type()->type;
		return;
	}

	// enum type
	auto enm = resolver.get_enum(type_name);
	if (enm.ambiguous()) {
		auto names = enm.results_names(true);
		std::string names_string = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, custom->position, type_name.c_str(), names_string.c_str());
		return;
	}
	else if (decl.found()) {
		custom = enm.first()->as_type()->type;
		return;
	}

	// unknown
	ERROR(ERR_UNKNOWN_TYPE, custom->position, type_name.c_str());
	return;
}
