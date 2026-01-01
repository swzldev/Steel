#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <filesystem>

// vars_file
//
// represents a file containing key value pairs usually used for storing
// cached variables such as the Windows SDK path.
//
// the format is entirely text based and each line contains a single key value pair
// e.g. `KEY=VALUE`

class vars_file {
public:
	vars_file() = default;

	bool load_from_file(const std::filesystem::path& filepath);
	bool save_to_file(const std::filesystem::path& filepath) const;

	// KEY=VALUE
	void set_var(const std::string& key, const std::string& value);
	bool get_var(const std::string& key, std::string& out_value) const;

	// KEY=VALUE1;VALUE2;VALUE3
	void set_var(const std::string& key, const std::vector<std::string>& values);
	bool get_var(const std::string& key, std::vector<std::string>& out_values) const;

private:
	std::unordered_map<std::string, std::string> vars;
};
