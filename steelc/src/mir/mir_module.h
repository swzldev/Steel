#pragma once

#include <string>
#include <vector>

#include <mir/mir_function.h>

struct mir_module {
	std::vector<mir_function> functions;
};