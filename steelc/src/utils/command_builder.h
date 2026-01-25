#pragma once

#include <string>
#include <vector>

class command_builder {
public:
	command_builder(const std::string& base_command)
		: command(base_command) {
	}

	inline command_builder& operator<<(const std::string& arg) {
		arguments.push_back(arg);
		return *this;
	}

	std::string build() const;

private:
	std::string command;
	std::vector<std::string> arguments;

	std::string escape(const std::string& str) const;
};