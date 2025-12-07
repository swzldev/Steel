#include "symbol_resolver.h"

#include "../entities/variable_entity.h"
#include "../entities/function_entity.h"
#include "../entities/type_entity.h"
#include "../entities/module_entity.h"

lookup_result symbol_resolver::get_variable(std::shared_ptr<const type_declaration> type, const std::string& name) const {
	lookup_result hierarchy_result = {};

	// search type provided first
	while (type != nullptr) {
		for (const auto& member : type->fields) {
			if (member->identifier == name) {
				// once found, dont keep looking
				return { variable_entity::make(member) };
			}
		}
		type = type->base_class;
	}

	// search through module hierarchy
	// note: dont bother passing type as weve already searched it
	std::shared_ptr<module_info> module = current_module;
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_variable(nullptr, name);
		if (hierarchy_result.found()) {
			return hierarchy_result;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_variable(nullptr, name);
		if (result.found()) {
			import_results.push_back(result);
		}
	}
	if (import_results.empty()) {
		// early out for no results
		return {};
	}
	else {
		// combine results (for collision handling later if needed)
		for (const auto& res : import_results) {
			auto& results = hierarchy_result.results;
			results.insert(results.end(), res.results.begin(), res.results.end());
		}
		return hierarchy_result;
	}

	return {};
}
lookup_result symbol_resolver::get_function(const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const {
	lookup_result hierarchy_result = {};

	// search through module hierarchy
	std::shared_ptr<module_info> module = current_module;
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_function(name, return_type, param_types);
		if (hierarchy_result.found()) {
			return hierarchy_result;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_function(name, return_type, param_types);
		if (result.found()) {
			import_results.push_back(result);
		}
	}
	if (import_results.empty()) {
		// early out for no results
		return {};
	}
	else {
		// combine results (for collision handling later if needed)
		for (const auto& res : import_results) {
			auto& results = hierarchy_result.results;
			results.insert(results.end(), res.results.begin(), res.results.end());
		}
		return hierarchy_result;
	}

	return {};
}
lookup_result symbol_resolver::get_method(std::shared_ptr<const type_declaration> type, const std::string& name, type_ptr return_type, std::vector<type_ptr> param_types) const {
	// search up type hierarchy
	while (type != nullptr) {
		for (const auto& m : type->methods) {
			if (m->identifier == name && *m->return_type == return_type) {
				if (m->parameters.size() == param_types.size()) {
					bool params_match = true;
					for (size_t i = 0; i < m->parameters.size(); ++i) {
						if (*m->parameters[i]->type != param_types[i]) {
							params_match = false;
							break;
						}
					}
					if (params_match) {
						return { function_entity::make(m) };
					}
				}
			}
		}
		type = type->base_class;
	}

	return {};
}
lookup_result symbol_resolver::get_type(const std::string& name) const {
	std::shared_ptr<module_info> module = current_module;
	lookup_result hierarchy_result = {};

	// search through module hierarchy
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_type(name);
		if (hierarchy_result.found()) {
			return hierarchy_result;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_type(name);
		if (result.found()) {
			import_results.push_back(result);
		}
	}
	if (import_results.empty()) {
		// early out for no results
		return {};
	}
	else {
		// combine results (for collision handling later if needed)
		for (const auto& res : import_results) {
			auto& results = hierarchy_result.results;
			results.insert(results.end(), res.results.begin(), res.results.end());
		}
		return hierarchy_result;
	}

	return {};
}
lookup_result symbol_resolver::get_enum(const std::string& name) const {
	std::shared_ptr<module_info> module = current_module;
	lookup_result hierarchy_result = {};

	// search through module hierarchy
	for (; module != nullptr; module = module->parent_module) {
		hierarchy_result = module->symbols.get_enum(name);
		if (hierarchy_result.found()) {
			return hierarchy_result;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols.get_enum(name);
		if (result.found()) {
			import_results.push_back(result);
		}
	}
	if (import_results.empty()) {
		// early out for no results
		return {};
	}
	else {
		// combine results (for collision handling later if needed)
		for (const auto& res : import_results) {
			auto& results = hierarchy_result.results;
			results.insert(results.end(), res.results.begin(), res.results.end());
		}
		return hierarchy_result;
	}

	return {};
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

