#include "steelc_command_manager.h"

#include <string>

#include <input/commands/flags/command_flags.h>
#include <input/commands/steelc_command.h>
#include <input/commands/steelc_commands_impl.h>
#include <input/commands/steelc_command_id.h>

const steelc_command* steelc_command_manager::get_command(const std::string& name) const {
	auto it = commands.find(name);
	if (it != commands.end()) {
		return &it->second;
	}
	return nullptr;
}

void steelc_command_manager::register_all() {
	// REGISTER ALL COMMANDS HERE
	register_command({
		"--help",
		steelc_commands_impl::help_command_handler,
		flag_config::from_command(STEELC_CMD_HELP)
	});
	register_command({
		"--version",
		steelc_commands_impl::version_command_handler,
		flag_config::from_command(STEELC_CMD_VERSION)
	});
	register_command({
		"build",
		steelc_commands_impl::build_command_handler,
		flag_config::from_command(STEELC_CMD_BUILD)
	});
	register_command({
		"project",
		steelc_commands_impl::project_command_handler,
		flag_config::from_command(STEELC_CMD_PROJECT)
	});
}
