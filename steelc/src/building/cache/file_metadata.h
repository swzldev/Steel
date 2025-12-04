#pragma once

#include <filesystem>
#include <cstdint>

class file_metadata {
public:
	file_metadata() = default;

	// *RELATIVE* path
	std::filesystem::path path;
	uint64_t last_modified = 0;
	uint64_t hash = 0;
	uint64_t size = 0;
};