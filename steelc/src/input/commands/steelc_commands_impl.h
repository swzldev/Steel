#pragma once

#include "../console_args.h"

namespace steelc_commands_impl {
	bool help_command_handler(const console_args& args);
	bool version_command_handler(const console_args& args);
	bool build_command_handler(const console_args& args);
	bool project_command_handler(const console_args& args);
}