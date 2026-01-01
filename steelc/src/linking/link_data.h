#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <linking/link_config.h>

struct link_data {
	std::vector<std::string> object_files;
	std::string object_format;
	link_config cfg;
	std::unordered_map<std::string, std::string> cached_vars;
};