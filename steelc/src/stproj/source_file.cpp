#include "source_file.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

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
	}
	else {
		throw std::runtime_error("Could not open source file: " + path);
	}
}