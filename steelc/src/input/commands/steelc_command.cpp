#include "steelc_command.h"

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>

#include <input/commands/flags/command_flags.h>
#include <input/commands/flags/flag_config.h>
#include <input/console_args.h>
#include <output/output.h>
#include <utils/console_colors.h>

bool steelc_command::execute(const console_args& args) const {
	command_flags flags({});

	if (!parse_args(args, flags)) {
		return false;
	}

	if (!handler) {
		return false;
	}

	// handle global flags
	if (flags.has("--verbose")) {
		output::set_log_verbosity(LOG_VERBOSITY_HIGH);
	}

	return handler(flags);

	if (flags.has("--verbose")) {
		output::set_log_verbosity(LOG_VERBOSITY_NORMAL);
	}
}

bool steelc_command::parse_args(const console_args& args, command_flags& out_flags) const {
	std::unordered_set<std::string> seen_flags;
	std::unordered_map<std::string, std::vector<std::string>> parsed_flags;

	// should probably move this into a dedicated function or something
	static const std::unordered_set<std::string> global_flags = {
		"--verbose"
	};
	
	const auto& cfg_flags = flag_cfg->get_flags();
	const auto& req_values = flag_cfg->get_required_values();

	auto arg_it = args.begin();

	for (auto rv_it = req_values.begin(); rv_it != req_values.end(); rv_it++) {
		if (arg_it == args.end()) {
			output::err("Error: missing required value: {}\n", console_colors::RED, *rv_it);
			return false;
		}
		parsed_flags[*rv_it] = { *arg_it++ };
	}

	while (arg_it != args.end()) {
		const std::string& arg = *arg_it;

		bool is_global = global_flags.contains(arg);

		if (!cfg_flags.contains(arg) && !is_global) {
			output::print("Warn: unknown flag: {}\n", console_colors::YELLOW, arg);
			++arg_it;
			continue;
		}

		if (seen_flags.contains(arg)) {
			output::err("Error: duplicate flag: {}\n", console_colors::RED, arg);
			return false;
		}

		if (is_global) {
			parsed_flags[arg] = {};
			seen_flags.insert(arg);
			++arg_it;
			continue;
		}

		int expected = cfg_flags.at(arg);
		++arg_it;

		std::vector<std::string> values;

		for (int i = 0; i < expected; i++) {
			if (arg_it == args.end()) {
				output::err("Error: flag {} expects {} value(s), but fewer were provided.\n", console_colors::RED, arg, expected);
				return false;
			}
			values.push_back(*arg_it);
			++arg_it;
		}

		parsed_flags[arg] = std::move(values);
		seen_flags.insert(arg);
	}

	out_flags = command_flags(flag_cfg, parsed_flags);
	return true;
}
