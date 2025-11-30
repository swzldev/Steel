#include "source_file.h"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <string>

source_file::source_file(const std::string& full_path, const std::string& relative_path) {
	this->relative_path = relative_path;
	this->full_path = full_path;

	std::ifstream file(full_path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
			content += line + "\n";
		}
		file.close();

		name_cache = std::filesystem::path(full_path).filename().string();
	}
	else {
		throw std::runtime_error("Could not open source file: " + relative_path);
	}
}

std::string source_file::name() const {
	return name_cache;
}
