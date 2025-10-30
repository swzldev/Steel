#include "symbol_table.h"

#include "../../error/error_catalog.h"

symbol_table::symbol_table() {
	scopes = {};
	scopes.emplace_back(); // global scope
}

void symbol_table::push_scope() {
	scopes.emplace_back();
	generic_scopes.emplace_back();
}
void symbol_table::pop_scope() {
	if (!scopes.empty()) {
		scopes.pop_back();
	}
	if (!generic_scopes.empty()) {
		generic_scopes.pop_back();
	}
}

error_code symbol_table::add_variable(std::shared_ptr<variable_declaration> var) {
	auto& current_scope = scopes.back();
	// variables cannot shadow generics
	for (const auto& gen_scope : generic_scopes) {
		if (gen_scope.find(var->identifier) != gen_scope.end()) {
			return ERR_VARIABLE_SHADOWS_GENERIC;
		}
	}
	if (current_scope.find(var->identifier) != current_scope.end()) {
		return ERR_VARIABLE_ALREADY_DECLARED_SCOPE;
	}
	current_scope[var->identifier] = var;
	return ERR_SUCCESS;
}
error_code symbol_table::add_generic(std::shared_ptr<generic_parameter> param) {
	auto& current_scope = generic_scopes.back();
	// generics cannot shadow variables
	for (const auto& var_scope : scopes) {
		if (var_scope.find(param->identifier) != var_scope.end()) {
			return ERR_GENERIC_SHADOWS_VARIABLE;
		}
	}
	if (current_scope.find(param->identifier) != current_scope.end()) {
		return ERR_DUPLICATE_GENERIC;
	}
	current_scope[param->identifier] = param;
	return ERR_SUCCESS;
}
error_code symbol_table::add_field(std::shared_ptr<type_declaration> type, std::shared_ptr<variable_declaration> var) {
	for (auto& field : type->fields) {
		if (field->identifier == var->identifier) {
			return ERR_VARIABLE_ALREADY_DECLARED_SCOPE; // field already exists
		}
	}
	type->fields.push_back(var);
	return ERR_SUCCESS;
}
error_code symbol_table::add_function(std::shared_ptr<function_declaration> func) {
	for (const auto& existing : functions) {
		const auto& existing_func = existing.second;
		if (existing.first == func->identifier) {
			// check params match
			if (existing_func->parameters.size() == func->parameters.size()) {
				bool params_match = true;
				for (size_t i = 0; i < func->parameters.size(); ++i) {
					if (*existing_func->parameters[i]->type != func->parameters[i]->type) {
						params_match = false;
						break;
					}
				}
				if (params_match) {
					// already defined
					return ERR_FUNC_ALREADY_DEFINED;
				}
			}

			if (*existing_func->return_type != func->return_type) {
				return ERR_RETURN_TYPE_CANT_OVERLOAD; // cannot overload by return type
			}
		}
	}
	functions.emplace_back(func->identifier, func);
	return ERR_SUCCESS;
}
error_code symbol_table::add_method(std::shared_ptr<type_declaration> type, std::shared_ptr<function_declaration> func) {
	// check if method already exists in type
	for (const auto& method : type->methods) {
		if (method->identifier == func->identifier) {
			// check params match
			if (method->parameters.size() == func->parameters.size()) {
				bool params_match = true;
				for (size_t i = 0; i < func->parameters.size(); ++i) {
					if (*method->parameters[i]->type != func->parameters[i]->type) {
						params_match = false;
						break;
					}
				}
				if (params_match) {
					return ERR_FUNC_ALREADY_DEFINED; // already defined
				}
			}
			if (*method->return_type != func->return_type) {
				return ERR_RETURN_TYPE_CANT_OVERLOAD; // cannot overload by return type
			}
		}
	}
	type->methods.push_back(func);
	return ERR_SUCCESS;
}
error_code symbol_table::add_type(std::shared_ptr<type_declaration> type) {
	if (types.find(type->name()) != types.end()) {
		return ERR_TYPE_ALREADY_DEFINED;
	}
	if (enums.find(type->name()) != enums.end()) {
		return ERR_ENUM_ALREADY_DEFINED;
	}
	types[type->name()] = type;
	return ERR_SUCCESS;
}
error_code symbol_table::add_enum(std::shared_ptr<enum_declaration> enm) {
	if (enums.find(enm->name()) != enums.end()) {
		return ERR_ENUM_ALREADY_DEFINED;
	}
	if (types.find(enm->name()) != types.end()) {
		return ERR_TYPE_ALREADY_DEFINED;
	}
	enums[enm->name()] = enm;
	return ERR_SUCCESS;
}

lookup_result symbol_table::get_variable(std::shared_ptr<const type_declaration> type, const std::string& name) const {
	// search up fields hierachy
	while (type != nullptr) {
		for (auto& field : type->fields) {
			if (field->identifier == name) {
				return { field };
			}
		}
		type = type->base_class;
	}
	// search globally
	for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
		auto found = it->find(name);
		if (found != it->end()) {
			return { found->second };
		}
	}
	return { LOOKUP_NO_MATCH };
}
lookup_result symbol_table::get_generic(const std::string& name) const {
	// local lookup
	for (auto it = generic_scopes.rbegin(); it != generic_scopes.rend(); ++it) {
		auto found = it->find(name);
		if (found != it->end()) {
			return { found->second };
		}
	}
}
lookup_result symbol_table::get_function(const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const {
	// local lookup
	for (const auto& func : functions) {
		if (func.first == name && *func.second->return_type == return_type) {
			for (size_t i = 0; i < func.second->parameters.size(); ++i) {
				if (i < param_types.size() && *func.second->parameters[i]->type == param_types[i]) {
					return { func.second };
				}
			}
		}
	}
	return { LOOKUP_NO_MATCH };
}
lookup_result symbol_table::get_type(const std::string& name) const {
	// local lookup
	auto it = types.find(name);
	if (it != types.end()) {
		return { it->second };
	}
	return { LOOKUP_NO_MATCH };
}
lookup_result symbol_table::get_enum(const std::string& name) const {
	// local lookup
	auto it = enums.find(name);
	if (it != enums.end()) {
		return { it->second };
	}
	return { LOOKUP_NO_MATCH };
}
std::vector<std::shared_ptr<function_declaration>> symbol_table::get_function_candidates(const std::string& name, size_t arity, size_t generics) const {
	std::vector<std::shared_ptr<function_declaration>> candidates;
	// local lookup
	for (const auto& func : functions) {
		if (func.first != name || func.second->parameters.size() != arity) {
			continue;
		}
		if (generics > 0 && func.second->generics.size() != generics) {
			continue;
		}
		candidates.push_back(func.second);
	}
	return candidates;
}