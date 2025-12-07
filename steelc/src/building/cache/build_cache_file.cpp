#include "build_cache_file.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>

#include "file_metadata.h"

bool build_cache_file::deserialize(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path)) {
		return false;
	}

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	// read and validate header
	char magic[4];
	file.read(magic, 4);
	if (std::string(magic, 4) != MAGIC) {
		return false;
	}

	// read (and validate) version
	file.read(reinterpret_cast<char*>(&version), sizeof(version));
	if (version < 0 || version > VERSION_LATEST) {
		return false;
	}

	// note for updating:
	// use the version variable to handle different versions here
	// build cache files should be fully backwards compatible

	if (version == 1) {
		// version 1 did not store steelc version
		steelc_version = STEELC_VERSION_PACKED;
	}
	else {
		file.read(reinterpret_cast<char*>(&steelc_version), sizeof(steelc_version));
	}

	uint32_t num_entries = 0;
	file.read(reinterpret_cast<char*>(&num_entries), sizeof(num_entries));
	for (uint32_t i = 0; i < num_entries; ++i) {
		uint32_t path_length = 0;
		file.read(reinterpret_cast<char*>(&path_length), sizeof(path_length));
		std::string path_str(path_length, '\0');
		file.read(&path_str[0], path_length);

		file_metadata meta;
		meta.path = path_str;
		file.read(reinterpret_cast<char*>(&meta.last_modified), sizeof(meta.last_modified));
		file.read(reinterpret_cast<char*>(&meta.hash), sizeof(meta.hash));
		file.read(reinterpret_cast<char*>(&meta.size), sizeof(meta.size));
		metadata[path_str] = meta;
	}

	file.close();
	return true;
}
bool build_cache_file::serialize(const std::filesystem::path& path) const {
	// cannot serialize outdated cache
	if (outdated()) {
		return false;
	}

	std::ofstream file(path, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	// write header
	file.write(MAGIC, 4);
	file.write(reinterpret_cast<const char*>(&version), sizeof(version));
	file.write(reinterpret_cast<const char*>(&steelc_version), sizeof(steelc_version));

	// number of entries
	uint32_t num_entries = static_cast<uint32_t>(metadata.size());
	file.write(reinterpret_cast<const char*>(&num_entries), sizeof(num_entries));

	for (const auto& [path_str, meta] : metadata) {
		uint32_t path_length = static_cast<uint32_t>(path_str.size());
		file.write(reinterpret_cast<const char*>(&path_length), sizeof(path_length));
		file.write(path_str.data(), path_length);
		file.write(reinterpret_cast<const char*>(&meta.last_modified), sizeof(meta.last_modified));
		file.write(reinterpret_cast<const char*>(&meta.hash), sizeof(meta.hash));
		file.write(reinterpret_cast<const char*>(&meta.size), sizeof(meta.size));
	}

	file.close();
	return true;
}

void build_cache_file::upgrade() {
	if (outdated()) {
		version = VERSION_LATEST;
		steelc_version = STEELC_VERSION_PACKED;
	}
}
