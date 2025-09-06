#pragma once

#include <string>
#include <vector>

class console_args {
public:
	console_args(int argc, char** argv);

	bool has_arg(const std::string& arg) const;

private:
	std::vector<std::string> args;
};