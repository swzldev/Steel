#include "symbol_resolver.h"

lookup_result symbol_resolver::get_variable(std::shared_ptr<const type_declaration> type, const std::string& name) const {
	std::shared_ptr<module_info> module = current_module;
	lookup_result hierarchy_result = { LOOKUP_NO_MATCH };

	// search type provided first
	while (type != nullptr) {
		for (const auto& member : type->fields) {
			if (member->identifier == name) {
				return { member };
			}
		}
		type = type->base_class;
	}

	// search through module hierarchy
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_variable(type, name);
		if (hierarchy_result.error == LOOKUP_OK) {
			break;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_variable(type, name);
		if (result.error == LOOKUP_OK) {
			import_results.push_back(result);
		}
	}

	// check for collisions
	if (hierarchy_result.error == LOOKUP_OK && !import_results.empty()) {
		return { LOOKUP_COLLISION };
	}

	if (import_results.size() > 1) {
		return { LOOKUP_COLLISION };
	}

	if (hierarchy_result.error == LOOKUP_OK) {
		return hierarchy_result;
	}
	if (import_results.size() == 1) {
		return import_results[0];
	}

	return { LOOKUP_NO_MATCH };
}
lookup_result symbol_resolver::get_function(const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const {
	std::shared_ptr<module_info> module = current_module;
	lookup_result hierarchy_result = { LOOKUP_NO_MATCH };

	// search through module hierarchy
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_function(name, return_type, param_types);
		if (hierarchy_result.error == LOOKUP_OK) {
			break;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_function(name, return_type, param_types);
		if (result.error == LOOKUP_OK) {
			import_results.push_back(result);
		}
	}

	// check for collisions
	if (hierarchy_result.error == LOOKUP_OK && !import_results.empty()) {
		return { LOOKUP_COLLISION };
	}

	if (import_results.size() > 1) {
		return { LOOKUP_COLLISION };
	}

	if (hierarchy_result.error == LOOKUP_OK) {
		return hierarchy_result;
	}
	if (import_results.size() == 1) {
		return import_results[0];
	}

	return { LOOKUP_NO_MATCH };
}
lookup_result symbol_resolver::get_method(std::shared_ptr<const type_declaration> type, const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const {
	std::shared_ptr<function_declaration> method = nullptr;

	for (const auto& method : type->methods) {
		if (method->identifier == name && *method->return_type == return_type) {
			if (method->parameters.size() == param_types.size()) {
				bool params_match = true;
				for (size_t i = 0; i < method->parameters.size(); ++i) {
					if (*method->parameters[i]->type != param_types[i]) {
						params_match = false;
						break;
					}
				}
				if (params_match) {
					return { method };
				}
			}
		}
	}

	return { LOOKUP_NO_MATCH };
}
lookup_result symbol_resolver::get_type(const std::string& name) const {
	std::shared_ptr<module_info> module = current_module;
	lookup_result hierarchy_result = { LOOKUP_NO_MATCH };

	// search through module hierarchy
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_type(name);
		if (hierarchy_result.error == LOOKUP_OK) {
			break;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_type(name);
		if (result.error == LOOKUP_OK) {
			import_results.push_back(result);
		}
	}

	// check for collisions
	if (hierarchy_result.error == LOOKUP_OK && !import_results.empty()) {
		return { LOOKUP_COLLISION };
	}

	if (import_results.size() > 1) {
		return { LOOKUP_COLLISION };
	}

	if (hierarchy_result.error == LOOKUP_OK) {
		return hierarchy_result;
	}
	if (import_results.size() == 1) {
		return import_results[0];
	}

	return { LOOKUP_NO_MATCH };
}
std::vector<std::shared_ptr<function_declaration>> symbol_resolver::get_function_candidates(const std::string& name, size_t arity, size_t generics) const {
	std::vector<std::shared_ptr<function_declaration>> candidates;

	// search through module hierarchy
	std::shared_ptr<module_info> module = current_module;
	for (; module != nullptr; module = module->parent_module) {
		auto funcs = module->symbols.get_function_candidates(name, arity, generics);
		candidates.insert(candidates.end(), funcs.begin(), funcs.end());
	}
	// search imports
	for (const auto& import : import_tbl->get_imports()) {
		auto funcs = import->symbols.get_function_candidates(name, arity, generics);
		candidates.insert(candidates.end(), funcs.begin(), funcs.end());
	}

	return candidates;
}
std::vector<std::shared_ptr<function_declaration>> symbol_resolver::get_ctor_candidates(const std::string& name, size_t arity) const {
	std::vector<std::shared_ptr<function_declaration>> candidates;

	const auto& result = get_type(name);
	if (result.error != LOOKUP_OK) {
		return candidates;
	}

	auto& type = std::get<std::shared_ptr<type_declaration>>(result.value);
	for (const auto& ctor : type->constructors) {
		if (ctor->parameters.size() == arity) {
			candidates.push_back(ctor);
		}
	}
	return candidates;
}
