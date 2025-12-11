#include "symbol_resolver.h"

#include <memory>
#include <string>
#include <vector>

#include <symbolics/lookup_result.h>
#include <symbolics/import_table.h>
#include <modules/module_info.h>
#include <representations/entities/entity.h>
#include <representations/entities/variable_entity.h>
#include <representations/entities/function_entity.h>
#include <representations/entities/type_entity.h>
#include <representations/entities/module_entity.h>

lookup_result symbol_resolver::lookup(const std::string& name, const std::shared_ptr<type_entity>& type) const {
	// search type members if provided
	if (type != nullptr) {
		auto ve = type->symbols.get_field(name);
		if (ve) {
			return { ve };
		}
		auto me = type->symbols.get_methods(name);
		if (!me.empty()) {
			lookup_result result;
			for (const auto& method : me) {
				result.results.push_back(method);
			}
			return result;
		}
	}

	// search through module hierarchy
	std::shared_ptr<module_entity> module = current_module;
	for (; module != nullptr; module = module->parent_module) {
		lookup_result result = module->symbols().lookup(name);
		if (result.found()) {
			return result;
		}
	}

	// search imports
	std::vector<lookup_result> import_results;
	for (const auto& import : import_tbl->get_imports()) {
		auto result = import->symbols().lookup(name);
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
		lookup_result result;
		for (const auto& res : import_results) {
			auto& results = result.results;
			results.insert(results.end(), res.results.begin(), res.results.end());
		}
		return result;
	}

	return {};
}
