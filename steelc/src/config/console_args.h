#pragma once

#include <string>
#include <vector>

class console_args {
public:
	console_args(int argc, char** argv);

	bool has_arg(const std::string& arg) const;
	inline std::string get_arg(size_t index) const {
		if (index < args.size()) {
			return args[index];
		}
		return "";
	}
	inline size_t count() const {
		return args.size();
	}

private:
	std::vector<std::string> args;
};