#include "string_utils.h"

#include <string>
#include <cstdarg>
#include <vector>

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
std::string string_utils::vec_to_string(const std::vector<std::string>& vec, const std::string& separator) {
	std::string result;
	for (size_t i = 0; i < vec.size(); ++i) {
		result += vec[i];
		if (i < vec.size() - 1) {
			result += separator;
		}
	}
	return result;
}
