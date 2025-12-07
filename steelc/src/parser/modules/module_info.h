#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "../symbolics/symbol_table.h"

using module_id = size_t;

struct module_info {
	module_id id;
	std::string name;
	symbol_table symbols;
	std::shared_ptr<module_info> parent_module;

	inline bool is_global() const {
		return parent_module == nullptr;
	}
	inline std::string full_name() const {
		if (is_global()) {
			return name;
		}
		return parent_module->full_name() + "::" + name;
	}
};