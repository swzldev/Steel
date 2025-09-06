#pragma once

#include <string>
#include <memory>
#include <filesystem>

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
};