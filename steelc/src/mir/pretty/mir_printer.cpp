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

	std::string full_name;
	for (const auto& scope : func.scopes) {
		full_name += scope + "::";
	}
	full_name += func.name;

	std::string params = "(";
	for (size_t pi = 0; pi < func.params.size(); pi++) {
		const auto& param = func.params[pi];
		if (param.name.empty()) {
			params += type_to_str(param.type);
		}
		else {
			params += type_to_str(param.type) + " " + param.name;
		}
		if (pi + 1 < func.params.size()) {
			params += ", ";
		}
	}
	params += ")";

	result += "func " + full_name + params + " {\n";

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
	case mir_instr_opcode::AND: return "and";
	case mir_instr_opcode::OR: return "or";
	case mir_instr_opcode::NOT: return "not";
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
	return std::visit([this](auto&& arg) -> std::string {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, mir_value>) {
			return value_to_str(arg);
		}
		else if constexpr (std::is_same_v<T, mir_const_int>) {
			return std::to_string(arg.value);
		}
		else if constexpr (std::is_same_v<T, mir_const_float>) {
			return std::to_string(arg.value);
		}
		else if constexpr (std::is_same_v<T, mir_string_imm>) {
			return "\"" + arg.value + "\"";
		}
		else if constexpr (std::is_same_v<T, mir_func_ref>) {
			std::string result;
			for (const auto& scope : arg.scopes) {
				result += scope + "::";
			}
			result += arg.name;
			return "func(" + result + ")";
		}
		else if constexpr (std::is_same_v<T, mir_field_ref>) {
			return "field(" + std::to_string(arg.index) + ")";
		}
		else {
			return "unknown_operand";
		}
	}, operand);
}
std::string mir_printer::value_to_str(const mir_value& value) {
	if (!value.valid()) {
		return "<invalid>";
	}
	if (!value.get_name().empty()) {
		return "%" + value.get_name(); // e.g. %var
	}
	return "%" + std::to_string(value.get_id()); // e.g. %2
}
std::string mir_printer::type_to_str(const mir_type& type) {
	// for now since its just a wrapper
	return type.ty->name();
}
