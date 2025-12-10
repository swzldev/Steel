#pragma once

#include <string>

#include "../symbolics/symbol_table.h"

using module_id = size_t;

struct module_info {
	module_id id;
	std::string name;
	symbol_table symbols;
};