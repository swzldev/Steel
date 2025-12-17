#pragma once

#include <string>

#include <mir/mir_fwd.h>
#include <mir/mir_operand.h>

// mir_printer
//
// class for pretty-printing MIR
// despite the name, it doesnt actually print anything
// it mostly just generates strings (typically for printing
// or writing to a file)
//
// note that MIR's text format is one way, it cannot be parsed
// back into MIR

class mir_printer {
public:
	std::string print_module(const mir_module& module);
	std::string print_function(const mir_function& func);
	std::string print_block(const mir_block& block);
	std::string print_instr(const mir_instr& instr);

private:
	std::string opcode_to_str(mir_instr_opcode opcode);
	std::string operand_to_str(const mir_operand& operand);
	std::string value_to_str(const mir_value& value);
	std::string type_to_str(const mir_type& type);
};