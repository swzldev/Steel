#include "steelc_command_manager.h"

#include <string>

#include "steelc_command.h"
#include "steelc_commands_impl.h"

const steelc_command* steelc_command_manager::get_command(const std::string& name) const {
	auto it = commands.find(name);
	if (it != commands.end()) {
		return &it->second;
	}
	return nullptr;
}

void steelc_command_manager::register_all() {
	// REGISTER ALL COMMANDS HERE
	register_command({ "--help", steelc_commands_impl::help_command_handler });
	register_command({ "--version", steelc_commands_impl::version_command_handler });
	register_command({ "build", steelc_commands_impl::build_command_handler });
	register_command({ "project", steelc_commands_impl::project_command_handler });
}
