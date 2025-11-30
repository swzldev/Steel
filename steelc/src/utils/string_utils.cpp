#include "string_utils.h"

#include <string>
#include <cstdarg>

std::string string_utils::format(const std::string& format_str, va_list args) {
	size_t size = std::vsnprintf(nullptr, 0, format_str.c_str(), args) + 1;
	if (size <= 0) {
		return "";
	}
	std::string result(size, '\0');
	std::vsnprintf(&result[0], size, format_str.c_str(), args);
	result.pop_back();
	return result;
}