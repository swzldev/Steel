#include "mir_builder.h"

#include <stdexcept>

#include <representations/types/data_type.h>
#include <representations/types/function_type.h>
#include <mir/mir_block.h>
#include <mir/mir_function.h>
#include <mir/mir_value.h>

void mir_builder::build_ret_void() {
	insert_instr({
		.kind = mir_instr_opcode::RET,
		.type = {data_type::get(DT_VOID)}
	});
}
void mir_builder::build_ret(mir_operand value) {
	insert_instr({
		.kind = mir_instr_opcode::RET,
		.type = operand_type(value),
		.operands = {
			mir_operand{value}
		}
	});
}

mir_value mir_builder::build_add(mir_operand lhs, mir_operand rhs) {
	return build_binary_op(mir_instr_opcode::ADD, lhs, rhs);
}
mir_value mir_builder::build_sub(mir_operand lhs, mir_operand rhs) {
	return build_binary_op(mir_instr_opcode::SUB, lhs, rhs);
}
mir_value mir_builder::build_mul(mir_operand lhs, mir_operand rhs) {
	return build_binary_op(mir_instr_opcode::MUL, lhs, rhs);
}
mir_value mir_builder::build_div(mir_operand lhs, mir_operand rhs) {
	return build_binary_op(mir_instr_opcode::DIV, lhs, rhs);
}
mir_value mir_builder::build_mod(mir_operand lhs, mir_operand rhs) {
	return build_binary_op(mir_instr_opcode::MOD, lhs, rhs);
}
mir_value mir_builder::build_binary_op(mir_instr_opcode opcode, mir_operand lhs, mir_operand rhs) {
	if (!check_type_match(lhs, rhs)) {
		throw std::runtime_error("Type mismatch in binary operation instruction");
	}

	// both same - just use lhs
	mir_value result = create_ssa_value(operand_type(lhs));
	insert_instr({
		.kind = opcode,
		.type = operand_type(lhs),
		.result = result,
		.operands = {
			mir_operand{lhs},
			mir_operand{rhs}
		}
		});
	return result;
}

mir_operand mir_builder::build_const_int(int64_t value, mir_type type) {
	return mir_const_int{
		.type = type,
		.value = value
	};
}
mir_operand mir_builder::build_const_float(double value, mir_type type) {
	return mir_const_float{
		.type = type,
		.value = value
	};
}
mir_operand mir_builder::build_const_string(const std::string& value, mir_type type) {
	return mir_string_imm{
		.type = type,
		.value = value
	};
}

mir_operand mir_builder::build_call(const std::vector<std::string>& scopes, const std::string& name, std::vector<mir_operand> args, mir_type func_type) {
	if (!func_type.ty->is_function()) {
		throw std::runtime_error("Function type provided to build_call is not a function type");
	}

	// insert function as first operand
	args.insert(args.begin(), mir_func_ref{ func_type, name, scopes });

	// create call instruction
	mir_value result = create_ssa_value(mir_type{ func_type.ty->as_function()->get_return_type() });
	insert_instr({
		.kind = mir_instr_opcode::CALL,
		.type = func_type,
		.result = result,
		.operands = args
	});
	return result;
}

mir_value mir_builder::build_cast(mir_operand value, mir_type target_type) {
	// shouldnt need to verify casting validity here - that should be done earlier
	mir_value result = create_ssa_value(target_type);
	insert_instr({
		.kind = mir_instr_opcode::CAST,
		.type = target_type,
		.result = result,
		.operands = {
			value
		}
	});
	return result;
}

void mir_builder::insert_instr(const mir_instr&& instr) {
	if (!ins_block) {
		throw std::runtime_error("Cannot insert an instruction when insert block is null");
	}
	ins_block->push_instr(instr);
}
mir_value mir_builder::create_ssa_value(mir_type type) {
	if (!func) {
		throw std::runtime_error("Cannot create a temporary value when function is null");
	}
	return func->make_value(type);
}

bool mir_builder::check_type_match(mir_operand lhs, mir_operand rhs) {
	auto lty = operand_type(lhs);
	auto rty = operand_type(rhs);
	if (!(lty == rty)) {
		return false;
	}
	return true;
}
