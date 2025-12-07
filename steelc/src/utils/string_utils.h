#pragma once

#include <string>
#include <cstdarg>
#include <vector>

namespace string_utils {
	std::string format(const std::string& format_str, va_list args);
	std::string vec_to_string(const std::vector<std::string>& vec, const std::string& separator = ", ");
}