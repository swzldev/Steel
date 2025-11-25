#pragma once

#include <string>

class output_config {
public:
	output_config() = default;

	std::string output_dir = "./build/";
	std::string intermediate_dir = "./build/.itm/";
};