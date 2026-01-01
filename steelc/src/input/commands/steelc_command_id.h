#pragma once

enum steelc_command_id {
	STEELC_CMD_HELP,			// --help
	STEELC_CMD_VERSION,			// --version
	STEELC_CMD_BUILD,			// build
	STEELC_CMD_PROJECT,			// project
	STEELC_CMD_TEST,			// test (STEELC_ENABLE_TESTS builds only)
};