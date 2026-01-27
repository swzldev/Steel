#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <vector>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>

// mir_operand
// 
// operands used in MIR instructions
// they can represent many different kinds of values such
// as variables, constants, function references, etc.

struct mir_const_int {
	mir_type type;
	int64_t value;
};

struct mir_const_float {
	mir_type type;
	double value;
};

struct mir_string_imm {
	mir_type type;
	std::string value;
};

struct mir_func_ref {
	mir_type type; // function type not return type
	std::vector<mir_type> generic_args;
	std::string name;
	std::vector<std::string> scopes;
};

struct mir_field_ref {
	uint32_t index;
};

using mir_operand = std::variant<
	mir_value,
	mir_const_int,
	mir_const_float,
	mir_string_imm,
	mir_func_ref,
	mir_field_ref
>;

inline mir_type operand_type(const mir_operand& op) {
	return std::visit([](auto&& arg) -> mir_type {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, mir_value>) {
			return arg.get_type();
		}
		else if constexpr (std::is_same_v<T, mir_const_int>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_const_float>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_string_imm>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_func_ref>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_field_ref>) {
			// field refs do not have a type themselves
			// their type is determined by the context in which they are used
			return mir_type{};
		}
		else {
			return mir_type{};
		}
	}, op);
}