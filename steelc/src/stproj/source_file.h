#pragma once

#include <string>
#include <vector>

class source_file {
public:
	source_file(const std::string& full_path, const std::string& relative_path);

	std::string name() const;

	std::string relative_path;
	std::string full_path;
	std::vector<std::string> lines;
	std::string content;

private:
	std::string name_cache;
};