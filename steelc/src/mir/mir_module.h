#pragma once

#include <string>
#include <vector>

#include <mir/mir_function.h>

// mir_module
//
// represents a MIR module containing multiple functions
// 
// note: mir_module does not represent modules in steel
// it is more closely related to an object file or a compilation unit

struct mir_module {
	std::string name; // typically the source file name without extension
	std::vector<mir_function> functions;
};