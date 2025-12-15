#pragma once

#include <string>

#include <mir/mir_instr.h>

struct mir_block {
	std::string name;
	std::vector<mir_instr> instructions;
};