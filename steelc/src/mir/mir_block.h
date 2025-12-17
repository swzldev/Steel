#pragma once

#include <string>

#include <mir/mir_instr.h>

// mir_block
//
// mir blocks are sequences of MIR instructions
// they represent basic blocks within a function

class mir_block {
public:
	mir_block(const std::string& name)
		: name(name) {
	}

	inline void push_instr(const mir_instr& instr) {
		instructions.push_back(instr);
	}
	inline const std::vector<mir_instr>& get_instrs() const {
		return instructions;
	}

	std::string name;

private:
	std::vector<mir_instr> instructions;
};