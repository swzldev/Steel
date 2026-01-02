#include "name_mangler.h"

#include <string>
#include <memory>
#include <vector>

#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/function_type.h>
#include <representations/types/container_types.h>
#include <representations/entities/module_entity.h>
#include <mir/mir_function.h>
#include <codegen/error/codegen_exception.h>

std::string name_mangler::mangle_function(const mir_function& fn_mir) {
	std::vector<mir_type> param_types;
	for (const auto& param : fn_mir.params) {
		param_types.push_back(param.type);
	}
	return mangle_function(fn_mir.name, fn_mir.scopes, param_types);
}
std::string name_mangler::mangle_function(const mir_func_ref& fn_ref_mir) {
	const auto& fn_type = fn_ref_mir.type.ty->as_function();
	if (!fn_type) {
		throw codegen_exception("Attempted to mangle non-function type as function");
	}

	std::vector<mir_type> param_types;
	for (const auto& pty : fn_type->get_parameter_types()) {
		param_types.push_back(mir_type{ pty });
	}

	return mangle_function(fn_ref_mir.name, fn_ref_mir.scopes, param_types);
}

std::string name_mangler::mangle_function(const std::string& name, const std::vector<std::string>& scopes, const std::vector<mir_type>& param_types) {
	// no mangle for bare-bone functions
	if (scopes.empty()) {
		return name;
	}

	std::string mangled = "_Z";

	// technically will always be true since empty scopes return the name immediately
	// might change this later
	if (scopes.size() > 0) {
		mangled += "N"; // start of nested name
		for (const auto& scope_name : scopes) {
			mangled += mangle_text(scope_name);
		}
	}

	// mangle function name
	mangled += mangle_text(name);

	// E for end of nested name
	if (scopes.size() > 0) {
		mangled += "E";
	}

	// currently this no longer works as function mir does not store
	// generics - will have to deal with this later

	// mangle generic params (if applicable)
	//if (func_ast->is_generic) {
	//	if (!func_ast->is_generic_instance) {
	//		throw codegen_exception("Attempted to mangle non-monomorphized function");
	//	}
	//	mangled += "I"; // start of generic args
	//	for (const auto& gen_arg : func_ast->generics) {
	//		mangled += mangle_type(gen_arg->substitution);
	//	}
	//	mangled += "E"; // end of generic args
	//}

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
std::string name_mangler::mangle_type(const mir_type& ty) {
	auto& t = ty.ty;
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
		// we will need to change some of this when adding nested type support
		if (custom->is_generic_instance()) {
			// generic type instance
			std::string mangled = "I"; // start of generic args
			for (const auto& gen_arg : custom->generic_args) {
				mangled += mangle_type(mir_type{ gen_arg }); // for now whilst mir_type is a wrapper
			}
			mangled += "E"; // end of generic args
			return mangle_text(custom->name()) + mangled;
		}
		return mangle_text(custom->name());
	}
	else if (auto ptr = t->as_pointer()) {
		return "P" + mangle_type(mir_type{ ptr->base_type });
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
