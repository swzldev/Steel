#pragma once

#include <mir/mir_fwd.h>
#include <mir/mir_operand.h>
#include <mir/mir_instr.h>

// mir_builder
// 
// utility class responsible for building MIR constructs

class mir_builder {
public:
	mir_builder() = default;

	inline void set_function(mir_function* function) {
		func = function;
	}
	inline void set_insert_block(mir_block* block) {
		ins_block = block;
	}

	// builder functions
	
	// return instructions
	void build_ret_void();
	void build_ret(mir_operand value);

	// arithmetic expressions
	mir_value build_add(mir_operand lhs, mir_operand rhs);
	mir_value build_sub(mir_operand lhs, mir_operand rhs);
	mir_value build_mul(mir_operand lhs, mir_operand rhs);
	mir_value build_div(mir_operand lhs, mir_operand rhs);
	mir_value build_mod(mir_operand lhs, mir_operand rhs);

	// literals/immediates/constants
	mir_operand build_const_int(int64_t value, mir_type type);
	mir_operand build_const_float(double value, mir_type type);
	mir_operand build_const_string(const std::string& value, mir_type type);

private:
	mir_function* func;
	mir_block* ins_block = nullptr;

	void insert_instr(const mir_instr&& instr);
	mir_value create_temp_value(mir_type type);

	// helpers
	bool check_type_match(mir_operand lhs, mir_operand rhs);

	// helper builders
	mir_value build_binary_op(mir_instr_opcode opcode, mir_operand lhs, mir_operand rhs);
};