#pragma once

#include "flags/command_flags.h"

namespace steelc_commands_impl {
	bool help_command_handler(const command_flags& flags);
	bool version_command_handler(const command_flags& flags);
	bool build_command_handler(const command_flags& flags);
	bool project_command_handler(const command_flags& flags);
};