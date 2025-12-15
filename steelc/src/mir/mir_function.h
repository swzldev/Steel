#pragma once

#include <string>
#include <vector>

#include <mir/mir_type.h>
#include <mir/mir_block.h>

struct mir_function {
	std::string mangled_name;
	mir_type return_type;
	std::vector<mir_type> param_types;
	std::vector<mir_block> blocks;
};