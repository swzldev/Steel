#pragma once

#include <string>
#include <vector>
#include <cstdint>

class source_file {
public:
	source_file(const std::string& full_path, const std::string& relative_path);

	std::string name() const;

	// filesystem utilites
	uint64_t get_last_modified_time() const;
	uint64_t get_size() const;

	std::string relative_path;
	std::string full_path;
	std::vector<std::string> lines;
	std::string content;

private:
	std::string name_cache;
};