#include "stproj_generator.h"

#include <fstream>
#include <string>
#include <filesystem>
#include <iostream>

#include "../output/output.h"

bool stproj_generator::generate_new_project(const std::string& name, const std::filesystem::path& location) {
	const auto project_dir = location / name;
	if (std::filesystem::exists(project_dir)) {
		output::err("A project with the same name already exists at: \"{}\"", "", project_dir.string());
		return false;
	}

	std::filesystem::create_directories(project_dir);

	if (!create_project_file(name, project_dir)) {
		output::err("Failed to create project file.\n");
		return false;
	}

	const auto src_dir = project_dir / "src";
	if (!std::filesystem::create_directory(src_dir)) {
		output::err("Failed to create source directory.\n");
		return false;
	}

	if (!create_main_file(src_dir)) {
		output::err("Failed to create main source file.\n");
		return false;
	}

	output::print("Successfully created new project \"{}\" at: {}\n", "", name, project_dir.string());
	return true;
}

bool stproj_generator::create_project_file(const std::string& name, const std::filesystem::path& path) {
	const std::filesystem::path project_file_path = path / (name + ".stproj");
	std::ofstream project_file(project_file_path);
	if (!project_file.is_open()) {
		return false;
	}

	project_file << "name = \"" << name << "\"\n";
	project_file << "version = \"0.1.0\"\n";
	project_file << "type = \"executable\"\n\n";

	project_file << "sources = [ \"src/main.st\" ]";

	return true;
}
bool stproj_generator::create_main_file(const std::filesystem::path& src_dir) {
	const auto main_file_path = src_dir / "main.st";
	std::ofstream main_file(main_file_path);
	if (!main_file.is_open()) {
		return false;
	}

	main_file << "func main() -> int {\n";
	main_file << "\t// TODO: Implement your program logic here\n";
	main_file << "\treturn 0;\n";
	main_file << "}\n";

	return true;
}
