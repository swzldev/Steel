#pragma once

#include <functional>
#include <string>

#include "flags/command_flags.h"
#include "flags/flag_config.h"
#include "../console_args.h"

class steelc_command {
public:
	using handler_type = std::function<bool(const command_flags&)>;

public:
	steelc_command(std::string name, const handler_type& handler, const flag_config* flag_cfg)
		: name(name), handler(handler), flag_cfg(flag_cfg) {
	}

	inline const std::string& get_name() const {
		return name;
	}

	bool execute(const console_args& args) const;

private:
	std::string name;
	handler_type handler;
	const flag_config* flag_cfg;

	bool parse_args(const console_args& args, command_flags& out_flags) const;
};