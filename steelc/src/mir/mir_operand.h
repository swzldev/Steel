#pragma once

#include <variant>
#include <string>

#include <mir/mir_value.h>

struct mir_const_int {
	int64_t value;
	int bits;
	bool is_signed;
};

struct mir_const_float {
	double value;
	bool is_double;
};

struct mir_func_ref {
	std::string mangled;
};

struct mir_field_ref {
	uint32_t index;
};

using mir_operand = std::variant<
	mir_value,
	mir_const_int,
	mir_const_float,
	std::string,
	mir_func_ref,
	mir_field_ref
>;