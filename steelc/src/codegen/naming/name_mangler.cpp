#include "name_mangler.h"

#include <string>
#include <memory>
#include <vector>

#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <representations/entities/module_entity.h>
#include <ast/declarations/function_declaration.h>
#include <codegen/error/codegen_exception.h>

std::string name_mangler::mangle_function(std::shared_ptr<function_declaration> func_ast) {
	// no mangle for bare-bone functions
	if (func_ast->parent_module->is_global() && !func_ast->is_method && !func_ast->is_constructor && !func_ast->is_generic) {
		return func_ast->identifier;
	}

	std::string mangled = "_Z";

	std::vector<std::string> scopes;

	// we keep this vector for additional hidden parameters
	// e.g. a this ptr in methods
	std::vector<type_ptr> param_types;
	for (const auto& param : func_ast->parameters) {
		param_types.push_back(param->type);
	}

	if (func_ast->is_method) {

	}
	else if (func_ast->is_constructor) {
	
	}
	else {
		// collect module scopes
		if (func_ast->parent_module && !func_ast->parent_module->is_global()) {
			std::shared_ptr<module_entity> mod_it = func_ast->parent_module;
			for (; mod_it && !mod_it->is_global(); mod_it = mod_it->parent_module) {
				scopes.insert(scopes.begin(), mod_it->name());
			}
		}
		// TODO: handle more than just module scopes (e.g. classes)
	}

	if (scopes.size() > 0) {
		mangled += "N"; // start of nested name
		for (const auto& scope_name : scopes) {
			mangled += mangle_text(scope_name);
		}
	}

	// mangle function name
	mangled += mangle_text(func_ast->identifier);

	// E for end of nested name
	if (scopes.size() > 0) {
		mangled += "E";
	}

	// mangle generic params (if applicable)
	if (func_ast->is_generic) {
		if (!func_ast->is_generic_instance) {
			throw codegen_exception("Attempted to mangle non-monomorphized function");
		}
		mangled += "I"; // start of generic args
		for (const auto& gen_arg : func_ast->generics) {
			mangled += mangle_type(gen_arg->substitution);
		}
		mangled += "E"; // end of generic args
	}

	// mangle parameter types
	for (const auto& pty : param_types) {
		mangled += mangle_type(pty);
	}

	return mangled;
}

std::string name_mangler::mangle_text(const std::string& text) {
	// length prefixed mangling, e.g. foo -> 3foo
	return std::to_string(text.length()) + text;
}
std::string name_mangler::mangle_type(type_ptr t) {
	if (t->is_primitive()) {
		switch (t->primitive) {
			case DT_I16:
				return "s";
			case DT_I32:
				return "i";
			case DT_I64:
				return "x";
			case DT_FLOAT:
				return "f";
			case DT_DOUBLE:
				return "d";
			case DT_CHAR:
				return "c";
			case DT_WIDECHAR:
				return "w";
			case DT_STRING:
				return "Pc"; // char*
			case DT_WIDESTRING:
				return "Pw"; // wchar*
			case DT_BYTE:
				return "h";
			case DT_BOOL:
				return "b";
			case DT_VOID:
				return "v";
		}
	}
	else if (auto custom = t->as_custom()) {
		return mangle_text(custom->name());
		// we will need to change this when adding nested type support
	}
	else if (auto ptr = t->as_pointer()) {
		return "P" + mangle_type(ptr->base_type);
	}
	else if (auto ref = t->as_reference()) {
		// doesnt exist yet
		//return "R" + mangle_type(ref->base_type);
	}
	else if (t->is_array()) {
		//auto array_t = t->as_array();
		//return "A" + std::to_string(array_t->size) + "_" + mangle_type(array_t->element_type);
	}
	else if (t->is_generic()) {
		// this should not happen, as all generic types should be
		// monomorphized before any codegen
		throw codegen_exception("Cannot mangle generic type parameter");
	}

	return "";
}
