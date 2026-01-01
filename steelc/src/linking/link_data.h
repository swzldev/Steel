#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <linking/link_config.h>
#include <building/cache/vars_file.h>

struct link_data {
	std::vector<std::string> object_files;
	std::string object_format;
	link_config cfg;
	vars_file& cached_vars;
};