#include "type_resolver.h"

#include <ast/ast.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/generic_param_entity.h>
#include <representations/entities/type_entity.h>
#include <representations/entities/module_entity.h>
#include <symbolics/lookup_result.h>
#include <symbolics/symbol_error.h>
#include <error/error_catalog.h>
#include <utils/string_utils.h>

void type_resolver::visit(std::shared_ptr<function_declaration> func) {
	sym_table->push_scope();
	sym_table->push_generic_scope();
	// add generics to the current scope
	for (const auto& generic : func->generics) {
		generic->param_index = cur_generic_index++;
		sym_table->add_symbol(generic);

		// naming errors should occur in earlier passes
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
	sym_table->pop_generic_scope();
	sym_table->pop_scope();
}
void type_resolver::visit(std::shared_ptr<variable_declaration> var) {
	// resolve variable type
	resolve_type(var->type);
}
void type_resolver::visit(std::shared_ptr<type_declaration> decl) {
	sym_table->push_scope();
	sym_table->push_generic_scope();
	// add generics to the current scope
	for (const auto& generic : decl->generics) {
		generic->param_index = cur_generic_index++;
		sym_table->add_symbol(generic);
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
	sym_table->pop_generic_scope();
	sym_table->pop_scope();
}
void type_resolver::visit(std::shared_ptr<module_declaration> decl) {
	resolver.current_module = decl->entity;
	for (auto& decl : decl->declarations) {
		decl->accept(*this);
	}
	resolver.current_module = decl->entity->parent_module;
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

	// lookup type
	auto t = sym_table->lookup(type_name);
	if (t.ambiguous()) {
		auto names = t.results_names(true);
		std::string names_string = string_utils::vec_to_string(names);
		ERROR(ERR_NAME_COLLISION, custom->position, type_name.c_str(), names_string.c_str());
		return;
	}
	else if (t.no_match()) {
		// unknown
		ERROR(ERR_UNKNOWN_TYPE, custom->position, type_name.c_str());
		return;
	}

	switch (t.first()->kind()) {
	// generic parameter
	case ENTITY_GENERIC_PARAM: {
		auto ent = t.first()->as_generic_param();
		auto gn = std::make_shared<generic_type>(type_name);
		gn->generic_param_index = ent->declaration->param_index;
		custom = gn;
		return;
	}
	// custom type or enum
	case ENTITY_TYPE: {
		auto ent = t.first()->as_type();
		custom = ent->declaration->type();
		return;
	}
	// non-type (e.g. module/function)
	default: {
		ERROR(ERR_NOT_A_TYPE, custom->position, type_name.c_str());
		return;
	}
	}
}
