#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <stdexcept>

#include "flag_config.h"

class command_flags {
public:
	command_flags(std::unordered_map<std::string, std::vector<std::string>> parsed_flags)
		: config(nullptr), parsed_flags(parsed_flags) {
	}
	command_flags(const flag_config* arg_cfg, std::unordered_map<std::string, std::vector<std::string>> parsed_flags)
		: config(arg_cfg), parsed_flags(parsed_flags) {
	}

	inline bool has(const std::string& flag) const {
		return parsed_flags.find(flag) != parsed_flags.end();
	}

	inline std::vector<std::string> get(const std::string& flag) const {
		auto it = parsed_flags.find(flag);
		if (it != parsed_flags.end()) {
			return it->second;
		}
		throw std::runtime_error("Flag not found: " + flag);
	}
	inline std::string get_first(const std::string& flag) const {
		auto got = get(flag);
		if (!got.empty()) {
			return got[0];
		}
		throw std::runtime_error("Flag has no values: " + flag);
	}

	const flag_config* get_config() const {
		return config;
	}


private:
	const flag_config* config;
	std::unordered_map<std::string, std::vector<std::string>> parsed_flags;
};