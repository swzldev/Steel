#pragma once

#include <vector>

#include "error.h"

namespace error_printer {
	void print_errors(const std::vector<error>& errors);
	void print_error(const error& err);
}