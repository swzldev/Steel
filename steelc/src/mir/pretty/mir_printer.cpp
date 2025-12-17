#include "mir_printer.h"

#include <mir/mir_module.h>
#include <mir/mir_function.h>
#include <mir/mir_block.h>
#include <mir/mir_instr.h>

std::string mir_printer::print_module(const mir_module& module) {
	std::string result;
	for (const auto& func : module.functions) {
		result += print_function(func);
	}
	return result;
}
std::string mir_printer::print_function(const mir_function& func) {
	std::string result;

	result += "func " + func.name + " {\n";

	for (const auto& block : func.blocks) {
		result += print_block(block);
	}

	result += "}\n\n";
	return result;
}
std::string mir_printer::print_block(const mir_block& block) {
	std::string result;

	result += block.name + ":\n";
	for (const auto& instr : block.get_instrs()) {
		result += "  " + print_instr(instr) + "\n";
	}

	return result;
}
std::string mir_printer::print_instr(const mir_instr& instr) {
	std::string result;

	if (instr.result.valid()) {
		result += value_to_str(instr.result) + " = ";
	}

	result += opcode_to_str(instr.kind);
	for (size_t i = 0; i < instr.operands.size(); ++i) {
		if (i == 0) {
			result += " ";
		}
		else {
			result += ", ";
		}
		result += operand_to_str(instr.operands[i]);
	}

	return result;
}

std::string mir_printer::opcode_to_str(mir_instr_opcode opcode) {
	switch (opcode) {
	case mir_instr_opcode::NOP: return "nop";
	case mir_instr_opcode::ADD: return "add";
	case mir_instr_opcode::SUB: return "sub";
	case mir_instr_opcode::MUL: return "mul";
	case mir_instr_opcode::DIV: return "div";
	case mir_instr_opcode::MOD: return "mod";
	case mir_instr_opcode::CMP_EQ: return "cmp_eq";
	case mir_instr_opcode::CMP_NEQ: return "cmp_neq";
	case mir_instr_opcode::CMP_LT: return "cmp_lt";
	case mir_instr_opcode::CMP_LTE: return "cmp_lte";
	case mir_instr_opcode::CMP_GT: return "cmp_gt";
	case mir_instr_opcode::CMP_GTE: return "cmp_gte";
	case mir_instr_opcode::LOAD: return "load";
	case mir_instr_opcode::STORE: return "store";
	case mir_instr_opcode::ADDR_OF: return "addr_of";
	case mir_instr_opcode::DEREF: return "deref";
	case mir_instr_opcode::INDEX: return "index";
	case mir_instr_opcode::FIELD: return "field";
	case mir_instr_opcode::CAST: return "cast";
	case mir_instr_opcode::CALL: return "call";
	case mir_instr_opcode::RET: return "ret";
	case mir_instr_opcode::BRA: return "bra";
	case mir_instr_opcode::BRA_CND: return "bra_cnd";
	case mir_instr_opcode::PHI: return "phi";
	default: return "unknown_opcode";
	}
}
std::string mir_printer::operand_to_str(const mir_operand& operand) {
	return "";
}
std::string mir_printer::value_to_str(const mir_value& value) {
	if (!value.get_name().empty()) {
		return "%" + value.get_name(); // e.g. %var
	}
	return "%" + std::to_string(value.get_id()); // e.g. %2
}
