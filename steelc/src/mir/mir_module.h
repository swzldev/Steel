#pragma once

#include <string>
#include <vector>

#include <mir/mir_function.h>

// mir_module
//
// represents a MIR module containing multiple functions

struct mir_module {
	std::vector<mir_function> functions;
};