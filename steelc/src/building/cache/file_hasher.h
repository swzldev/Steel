#pragma once

#include <filesystem>
#include <cstdint>

class file_hasher {
public:
	// FNV-1a hash algorithm
	static uint64_t hash_file(const std::filesystem::path& path, uint64_t* file_size = nullptr);
};