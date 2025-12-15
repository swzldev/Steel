#pragma once

#include <vector>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>
#include <mir/mir_operand.h>

enum class mir_instr_kind {
	NOP,
	ADD, SUB, MUL, DIV, MOD,
	CMP_EQ, CMP_NEQ, CMP_LT, CMP_LTE, CMP_GT, CMP_GTE,
	LOAD, STORE,
	ADDR_OF, DEREF,
	INDEX, FIELD, CAST,
	CALL,
	RET,
	BRA, BRA_CND,
	PHI,
};

struct mir_instr {
	mir_instr_kind kind = mir_instr_kind::NOP;
	mir_type type{};
	mir_value result{};
	std::vector<mir_operand> operands;
};