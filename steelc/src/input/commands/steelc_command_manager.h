#pragma once

#include <string>
#include <unordered_map>

#include "steelc_command.h"

class steelc_command_manager {
public:
	inline steelc_command_manager() {
		register_all();
	}

	const steelc_command* get_command(const std::string& name) const;

private:
	std::unordered_map<std::string, steelc_command> commands;
	
	void register_all();
	inline void register_command(const steelc_command& command) {
		commands.emplace(command.get_name(), command);
	}
};