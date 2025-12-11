#pragma once

#include <string>
#include <map>
#include <vector>

#include <input/commands/steelc_command_id.h>

class flag_config {
public:
	static const flag_config* from_command(steelc_command_id cmd_id);

	const std::map<const std::string, int>& get_flags() const {
		return flags;
	}
	const std::vector<std::string>& get_required_values() const {
		return required_values;
	}

private:
	flag_config() = default;
	flag_config(const std::vector<std::string>& req_vals, const std::map<const std::string, int>& flags)
		: required_values(req_vals), flags(flags) {
	}
	flag_config(const std::map<const std::string, int>& flags)
		: required_values({}), flags(flags) {
	}
	flag_config(const flag_config&) = default;
	flag_config& operator=(const flag_config&) = default;

	std::vector<std::string> required_values;
	std::map<const std::string, int> flags;
};