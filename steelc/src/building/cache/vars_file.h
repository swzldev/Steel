#pragma once

#include <string>
#include <unordered_map>
#include <memory>

// vars_file
//
// represents a file containing key value pairs usually used for storing
// cached variables such as the Windows SDK path.
//
// the format is entirely text based and each line contains a single key value pair
// e.g. `KEY=VALUE`

class vars_file {
public:
	bool load_from_file(const std::string& filepath);
	bool save_to_file(const std::string& filepath) const;

	void set_var(const std::string& key, const std::string& value);
	bool get_var(const std::string& key, std::string& out_value) const;

private:
	vars_file() = default;

	std::unordered_map<std::string, std::string> vars;
};
