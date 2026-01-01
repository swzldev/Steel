#pragma once

#include <string>

#include <sys/target_triple.h>

struct link_config {
	// output settings
	std::string output_dir;
	std::string output_name;

	// linking settings
	target_triple target;
};