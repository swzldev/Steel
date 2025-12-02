#pragma once

#include <string>
#include <filesystem>

class stproj_generator {
public:
	bool generate_new_project(const std::string& name, const std::filesystem::path& location);

private:
	bool create_project_file(const std::string& name, const std::filesystem::path& path);
	bool create_main_file(const std::filesystem::path& src_dir);
};