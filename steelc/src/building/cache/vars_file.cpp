#include "vars_file.h"

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <unordered_map>

bool vars_file::load_from_file(const std::string& filepath) {
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
bool vars_file::save_to_file(const std::string& filepath) const {
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
