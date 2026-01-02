#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>
#include <mir/mir_block.h>

struct mir_function_param {
	mir_type type;
	std::string name;
	mir_value value;
};

class mir_function{
public:
	mir_function() = default;

	inline mir_block* get_entry_block() {
		if (blocks.empty()) {
			return nullptr;
		}
		return &blocks[0];
	}

	inline mir_value make_value(const mir_type& ty, const std::string& name = "") {
		return mir_value(next_value_id++, ty, name);
	}

	std::string name;
	std::vector<std::string> scopes;
	mir_type return_type;
	std::vector<mir_function_param> params;
	std::vector<mir_block> blocks;

private:
	uint32_t next_value_id = 0;
};