#include "source_file.h"

#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>

source_file::source_file(const std::string& path) {
	std::ifstream file(path);
	if (file.is_open()) {
		this->path = path;
		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
			content += line + "\n";
		}
		file.close();

		name_cache = std::filesystem::path(path).filename().string();
	}
	else {
		throw std::runtime_error("Could not open source file: " + path);
	}
}

std::string source_file::name() const {
	return name_cache;
}
