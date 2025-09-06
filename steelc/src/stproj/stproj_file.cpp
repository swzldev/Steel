#include "stproj_file.h"

#include <filesystem>
#include <toml++/toml.hpp>

stproj_file stproj_file::load(std::string path) {
	stproj_file proj;
	auto file = toml::parse_file(path);
	auto root = std::filesystem::path(path).parent_path();

	proj.project_name = file["name"].value_or("unknown");
	proj.project_version = file["version"].value_or("unknown");
	proj.project_type = file["type"].value_or("unknown");

	if (auto sources = file["sources"].as_array()) {
		for (const auto& source : *sources) {
			if (source.is_string()) {
				auto full_path = root / source.as_string()->get();
				proj.sources.push_back(source_file(full_path.string()));
			}
		}
	}

	if (auto deps = file["dependencies"].as_table()) {
		for (const auto& [name, path] : *deps) {
			if (path.is_string()) {
				proj.dependencies.push_back({ std::string(name.str()), path.as_string()->get() });
			}
		}
	}
	return proj;
}
void stproj_file::save(const std::string& path)
{
}