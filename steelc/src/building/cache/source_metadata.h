#pragma once

#include <filesystem>
#include <cstdint>

class source_metadata {
public:
	source_metadata() = default;

	// *RELATIVE* path
	std::string path;
	uint64_t last_modified = 0;
	uint64_t hash = 0;
	uint64_t size = 0;
};