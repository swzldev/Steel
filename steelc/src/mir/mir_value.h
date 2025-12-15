#pragma once

#include <cstdint>
#include <string>

#include <mir/mir_type.h>

struct mir_value {
	uint32_t id = 0;
	mir_type type{};
	std::string name; // for debugging
};
