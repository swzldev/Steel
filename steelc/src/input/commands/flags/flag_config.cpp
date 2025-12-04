#include "flag_config.h"

#include <unordered_map>

#include "../steelc_command_id.h"

const flag_config* flag_config::from_command(steelc_command_id cmd_id) {
	static const flag_config default_config;

	if (cmd_id == STEELC_CMD_HELP) {
		return &default_config;
	}
	else if (cmd_id == STEELC_CMD_VERSION) {
		return &default_config;
	}

	else if (cmd_id == STEELC_CMD_BUILD) {
		static const flag_config cfg{
			{ "<project-path>" },
			{
				{ "--no-link", 0 },
				{ "--all", 0 },
				{ "--out", 1 },
				{ "--int", 1 },
			}
		};
		return &cfg;
	}

	else if (cmd_id == STEELC_CMD_PROJECT) {
		static const flag_config cfg{
			{ "<option>", "<project>" },
			{
			}
		};
		return &cfg;
	}
	
	return nullptr;
}
