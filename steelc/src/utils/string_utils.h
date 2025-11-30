#pragma once

#include <string>
#include <cstdarg>

namespace string_utils {
	std::string format(const std::string& format_str, va_list args);
}