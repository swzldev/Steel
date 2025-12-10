#pragma once

#include <filesystem>
#include <cstdint>
#include <unordered_map>
#include <string>

#include "file_metadata.h"
#include "../../steelc/steelc_definitions.h"

class build_cache_file {
public:
	static constexpr auto EXTENSION = ".dat";
	static constexpr auto MAGIC = "SCBC";
	static constexpr int VERSION_LATEST = 2;

public:
	build_cache_file() = default;

	bool deserialize(const std::filesystem::path& path);
	bool serialize(const std::filesystem::path& path) const;

	inline bool outdated() const {
		return version != VERSION_LATEST || steelc_version != STEELC_VERSION_PACKED_U32;
	}
	void upgrade();

	inline std::unordered_map<std::string, file_metadata>& get_metadata() {
		return metadata;
	}
	inline void set_metadata(const std::string& path, const file_metadata& meta) {
		metadata[path] = meta;
	}
	inline void remove_metadata(const std::string& path) {
		metadata.erase(path);
	}

private:
	std::unordered_map<std::string, file_metadata> metadata;
	int version = VERSION_LATEST;
	uint32_t steelc_version = STEELC_VERSION_PACKED_U32;
};