#include "file_hasher.h"

#include <filesystem>
#include <fstream>
#include <cstdint>

uint64_t file_hasher::hash_file(const std::filesystem::path& path, uint64_t* file_size) {
	const uint64_t fnv_prime = 0x100000001B3;
	const uint64_t fnv_offset_basis = 0xCBF29CE484222325;

	uint64_t out_hash = fnv_offset_basis;
	uint64_t total_bytes = 0;

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		if (file_size) {
			*file_size = 0;
		}
		return 0;
	}

	char buffer[4096];
	while (true) {
		file.read(buffer, sizeof(buffer));
		std::streamsize n = file.gcount();
		if (n <= 0) break;

		total_bytes += static_cast<uint64_t>(n);

		for (std::streamsize i = 0; i < n; ++i) {
			out_hash ^= static_cast<uint8_t>(buffer[i]);
			out_hash *= fnv_prime;
		}

		if (!file) break;
	}

	if (file_size) {
		*file_size = total_bytes;
	}

	return out_hash;
}