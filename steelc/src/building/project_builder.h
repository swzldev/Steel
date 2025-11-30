#pragma once

#include <string>
#include <chrono>
#include <vector>

#include "../stproj/stproj_file.h"

class project_builder {
public:
	project_builder() = default;

	bool build_project(const std::string& project_path);
	
	int run_build_command(const std::string& command);

	// returns filename of the project (WITHOUT .stproj)
	inline std::string project_filename() {
		return project_file.filename_no_extension().string();
	}

	std::vector<std::string> post_build_commands;

private:
	stproj_file project_file;

	std::chrono::high_resolution_clock::time_point build_start;
	std::chrono::high_resolution_clock::time_point compile_start;

	std::vector<std::string> generated_ir_files;

	inline void mark_build_start() {
		build_start = std::chrono::high_resolution_clock::now();
	}
	inline void mark_compile_start() {
		compile_start = std::chrono::high_resolution_clock::now();
	}
	// elapsed time (IN SECONDS) since building started
	double get_build_time() const;
	// elapsed time (IN SECONDS) since compilation started
	double get_compilation_time() const;

	std::string replace_vars(const std::string& str);
};