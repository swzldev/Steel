#pragma once

#include <string>
#include <vector>

class console_args {
public:
	console_args(int argc, char** argv);

	bool has_arg(const std::string& arg) const;
	int find_arg(const std::string& arg) const;
	inline const std::string* get_arg(size_t index) const {
		if (index < args.size()) {
			return &args[index];
		}
		return nullptr;
	}
	inline size_t count() const {
		return args.size();
	}

private:
	std::vector<std::string> args;
};