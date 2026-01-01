#include "vars_file.h"

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

bool vars_file::load_from_file(const std::filesystem::path& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream line_stream(line);
		std::string key;
		if (std::getline(line_stream, key, '=')) {
			std::string value;
			if (std::getline(line_stream, value)) {
				vars[key] = value;
			}
		}
	}

	return true;
}
bool vars_file::save_to_file(const std::filesystem::path& filepath) const {
	// ensure directory exists
	fs::create_directories(filepath.parent_path());

	// output to file
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	for (const auto& [key, value] : vars) {
		file << key << "=" << value << "\n";
	}

	return true;
}

void vars_file::set_var(const std::string& key, const std::string& value) {
	vars[key] = value;
}
bool vars_file::get_var(const std::string& key, std::string& out_value) const {
	auto it = vars.find(key);
	if (it != vars.end()) {
		out_value = it->second;
		return true;
	}
	return false;
}

void vars_file::set_var(const std::string& key, const std::vector<std::string>& values) {
	std::string combined;
	for (size_t i = 0; i < values.size(); i++) {
		combined += values[i];
		if (i + 1 < values.size()) {
			combined += ";";
		}
	}
	vars[key] = combined;
}
bool vars_file::get_var(const std::string& key, std::vector<std::string>& out_values) const {
	auto it = vars.find(key);
	if (it != vars.end()) {
		std::istringstream iss(it->second);
		std::string value;
		while (std::getline(iss, value, ';')) {
			out_values.push_back(value);
		}
		return true;
	}
	return false;
}
