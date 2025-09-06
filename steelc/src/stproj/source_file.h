#pragma once

#include <string>
#include <vector>

class source_file {
public:
	source_file(const std::string& path);

	std::string path;
	std::vector<std::string> lines;
	std::string content;
};