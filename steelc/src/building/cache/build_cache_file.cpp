#include "build_cache_file.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>

#include <building/cache/source_metadata.h>
#include <building/cache/artifact_metadata.h>

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

	if (version <= 1) {
		// version <1 did not store steelc version
		steelc_version = STEELC_VERSION_PACKED_U32;
	}
	else {
		file.read(reinterpret_cast<char*>(&steelc_version), sizeof(steelc_version));
	}

	uint32_t num_src_entries = 0;
	file.read(reinterpret_cast<char*>(&num_src_entries), sizeof(num_src_entries));
	for (uint32_t i = 0; i < num_src_entries; ++i) {
		source_metadata meta;

		// path
		read_string(file, &meta.path);

		// last modified, hash, size
		file.read(reinterpret_cast<char*>(&meta.last_modified), sizeof(meta.last_modified));
		file.read(reinterpret_cast<char*>(&meta.hash), sizeof(meta.hash));
		file.read(reinterpret_cast<char*>(&meta.size), sizeof(meta.size));

		src_metadata[meta.path] = meta;
	}

	if (version <= 2) {
		// version <2 did not store artifact metadata
		art_metadata = {};
	}
	else {
		uint32_t num_art_entries = 0;
		file.read(reinterpret_cast<char*>(&num_art_entries), sizeof(num_art_entries));
		for (uint32_t i = 0; i < num_art_entries; ++i) {
			artifact_metadata meta;

			// path
			read_string(file, &meta.path);
			// kind
			file.read(reinterpret_cast<char*>(&meta.kind), sizeof(meta.kind));
			// src_relpath
			read_string(file, &meta.src_relpath);
			// name
			read_string(file, &meta.name);
			// extension
			read_string(file, &meta.extension);
			// format
			read_string(file, &meta.format);

			// attributes
			uint32_t num_attributes = 0;
			file.read(reinterpret_cast<char*>(&num_attributes), sizeof(num_attributes));
			for (uint32_t j = 0; j < num_attributes; ++j) {
				std::string key;
				std::string value;
				read_string(file, &key);
				read_string(file, &value);
				meta.attributes[key] = value;
			}

			// is_binary
			file.read(reinterpret_cast<char*>(&meta.is_binary), sizeof(meta.is_binary));

			art_metadata[meta.path] = meta;
		}
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
	uint32_t num_src_entries = static_cast<uint32_t>(src_metadata.size());
	file.write(reinterpret_cast<const char*>(&num_src_entries), sizeof(num_src_entries));

	for (const auto& [path_str, meta] : src_metadata) {
		// path
		write_string(file, path_str);

		// last modified, hash, size
		file.write(reinterpret_cast<const char*>(&meta.last_modified), sizeof(meta.last_modified));
		file.write(reinterpret_cast<const char*>(&meta.hash), sizeof(meta.hash));
		file.write(reinterpret_cast<const char*>(&meta.size), sizeof(meta.size));
	}

	for (const auto& [path_str, meta] : art_metadata) {
		// path
		write_string(file, meta.path);
		// kind
		file.write(reinterpret_cast<const char*>(&meta.kind), sizeof(meta.kind));
		// src_relpath
		write_string(file, meta.src_relpath);
		// name
		write_string(file, meta.name);
		// extension
		write_string(file, meta.extension);
		// format
		write_string(file, meta.format);

		// attributes
		uint32_t num_attributes = static_cast<uint32_t>(meta.attributes.size());
		file.write(reinterpret_cast<const char*>(&num_attributes), sizeof(num_attributes));
		for (const auto& [key, value] : meta.attributes) {
			write_string(file, key);
			write_string(file, value);
		}

		// is_binary
		file.write(reinterpret_cast<const char*>(&meta.is_binary), sizeof(meta.is_binary));
	}

	file.close();
	return true;
}

void build_cache_file::upgrade() {
	if (outdated()) {
		version = VERSION_LATEST;
		steelc_version = STEELC_VERSION_PACKED_U32;
	}
}

void build_cache_file::write_string(std::ofstream& file, const std::string& str) const {
	// basic format: [length, data]
	uint32_t length = static_cast<uint32_t>(str.size());
	file.write(reinterpret_cast<const char*>(&length), sizeof(length));
	file.write(str.data(), length);
}
void build_cache_file::read_string(std::ifstream& file, std::string* str) const {
	// basic format: [length, data]
	uint32_t length = 0;
	file.read(reinterpret_cast<char*>(&length), sizeof(length));
	str->resize(length);
	file.read(&(*str)[0], length);
}
