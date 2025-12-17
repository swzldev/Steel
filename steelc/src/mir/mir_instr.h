#pragma once

#include <vector>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>
#include <mir/mir_operand.h>

// mir_instr
//
// represents a single MIR instruction such as an operation, load/store, branch, etc.
// instructions have an opcode, a result value (if any), a type, and operands

enum class mir_instr_opcode {
	NOP,													// no operation
	ADD, SUB, MUL, DIV, MOD,								// arithmetic
	CMP_EQ, CMP_NEQ, CMP_LT, CMP_LTE, CMP_GT, CMP_GTE,		// comparison
	LOAD, STORE,											// memory operations
	ADDR_OF, DEREF,											// pointer operations
	INDEX, FIELD, CAST,										// data access and conversion
	CALL,													// function call
	RET,													// return from function
	BRA, BRA_CND,											// branching
	PHI,													// SSA
};

struct mir_instr {
	mir_instr_opcode kind = mir_instr_opcode::NOP;
	mir_type type{};
	mir_value result{};
	std::vector<mir_operand> operands;
};