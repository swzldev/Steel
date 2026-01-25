#include "command_builder.h"

#include <string>
#include <vector>

std::string command_builder::build() const {
	std::string full_command = escape(command);
	for (const auto& arg : arguments) {
		full_command += " " + escape(arg);
	}
	return full_command;
}

std::string command_builder::escape(const std::string& str) const {
	if (str.find(' ') != std::string::npos) {
		return "\"" + str + "\"";
	}
	return str;
}
