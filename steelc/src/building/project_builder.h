#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <filesystem>

#include "cache/file_metadata.h"
#include "cache/build_cache_file.h"
#include "build_config.h"
#include "code_outputter.h"
#include "../stproj/source_file.h"
#include "../stproj/stproj_file.h"

class project_builder {
public:
	project_builder(build_config cfg)
		: build_cfg(cfg) {
	}

	bool load_project(const std::string& project_path);
	bool build_project();
	
	int run_build_command(const std::string& command);

	// returns filename of the project (WITHOUT .stproj)
	inline std::string project_filename() {
		return project_file->filename_no_extension().string();
	}

	std::vector<std::string> post_build_commands;

private:
	build_config build_cfg;
	std::unique_ptr<stproj_file> project_file;
	std::unique_ptr<code_outputter> outputter;
	std::filesystem::path cache_path;

	std::chrono::high_resolution_clock::time_point build_start;
	std::chrono::high_resolution_clock::time_point compile_start;

	std::vector<std::string> all_irs;

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

	std::string get_ir_path(const source_file& src, bool relative = true) const;

	// build cache
	build_cache_file load_cache();
	void save_cache(const build_cache_file& cache) const;
	std::vector<source_file> get_files_to_compile(build_cache_file& cache);
	file_metadata generate_metadata(const source_file& src);

	std::string replace_vars(const std::string& str);
};