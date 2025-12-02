#pragma once

#include <functional>
#include <string>

#include "../console_args.h"

class steelc_command {
public:
	steelc_command(const std::string& name, const std::function<bool(const console_args& args)>& handler)
		: name(name), handler(handler) {
	}

	inline const std::string& get_name() const {
		return name;
	}
	inline bool execute(const console_args& args) const {
		if (!handler) {
			return false;
		}
		return handler(args);
	}

private:
	std::string name;
	std::function<bool(const console_args& args)> handler;
};