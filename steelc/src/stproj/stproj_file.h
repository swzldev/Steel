#pragma once

#define TOML_EXCEPTIONS 0

#include <vector>
#include <string>
#include <filesystem>
#include <memory>

#include <toml++/toml.hpp>

#include "source_file.h"

struct dependency {
	std::string name;
	std::string path;
};

class stproj_file {
public:
	// note: throws bad_stproj_exception on failure
	static std::unique_ptr<stproj_file> load(const std::string& path);
	void save(const std::string& path);

	std::filesystem::path filename() const;
	std::filesystem::path filename_no_extension() const;
	std::filesystem::path parent_path() const;

	std::string project_name;
	std::string project_version;
	std::string project_type;
	std::vector<source_file> sources;
	std::vector<dependency> dependencies;
	
private:
	stproj_file() = default;
	stproj_file(const stproj_file&) = delete;
	stproj_file& operator=(const stproj_file&) = delete;

	std::filesystem::path path;

	static const std::string require_string(const std::string& key, const toml::table& table);
};