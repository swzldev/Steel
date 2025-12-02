#include "stproj_file.h"

#include <filesystem>
#include <string>
#include <memory>
#include <toml++/toml.hpp>

#include "source_file.h"
#include "bad_stproj_exception.h"

std::unique_ptr<stproj_file> stproj_file::load(const std::string& path) {
	auto proj = std::unique_ptr<stproj_file>(new stproj_file());
	proj->path = std::filesystem::path(path);
	auto file = toml::parse_file(path);
	if (file.failed()) {
		throw bad_stproj_exception("Failed to parse .stproj file: " + std::string(file.error().description()));
	}
	auto& table = file.table();

	auto root = std::filesystem::path(path).parent_path();

	proj->project_name = require_string("name", table);
	proj->project_version = require_string("version", table);
	proj->project_type = require_string("type", table);

	if (auto sources = file["sources"].as_array()) {
		for (const auto& source : *sources) {
			if (source.is_string()) {
				std::string rel_path = source.as_string()->get();
				std::string full_path = (root / rel_path).string();
				proj->sources.push_back(source_file(full_path, rel_path));
			}
		}
	}
	else {
		throw bad_stproj_exception("Steel project must have a 'sources' array");
	}

	if (auto deps = file["dependencies"].as_table()) {
		for (const auto& [name, path] : *deps) {
			if (path.is_string()) {
				proj->dependencies.push_back({ std::string(name.str()), path.as_string()->get() });
			}
		}
	}
	return proj;
}
void stproj_file::save(const std::string& path) {
}

std::filesystem::path stproj_file::filename() const {
	return path.filename();
}
std::filesystem::path stproj_file::filename_no_extension() const {
	return filename().stem();
}
std::filesystem::path stproj_file::parent_path() const {
	return path.parent_path();
}

const std::string stproj_file::require_string(const std::string& key, const toml::table& table) {
	auto str = table[key];
	if (!str || !str.is_string()) {
		throw bad_stproj_exception("Steel project missing required key: \'" + key + "\'");
	}
	return str.value<std::string>().value();
}
