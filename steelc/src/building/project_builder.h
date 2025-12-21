#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <filesystem>
#include <cstdint>

#include <llvm/IR/Module.h>

#include <building/cache/source_metadata.h>
#include <building/cache/artifact_metadata.h>
#include <building/cache/build_cache_file.h>
#include <building/build_config.h>
#include <building/code_outputter.h>
#include <compiler/compiler.h>
#include <stproj/source_file.h>
#include <stproj/stproj_file.h>

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

	// gets the path (relative/non-relative) for a given code artifact
	// it uses the artifacts kind to determine the appropriate subdirectory
	std::string get_artifact_path(const code_artifact& artifact, bool relative = true) const;

	inline std::filesystem::path get_project_dir() const {
		return project_file->parent_path();
	}
	inline std::filesystem::path get_output_dir() const {
		return project_file->parent_path() / build_cfg.output_dir;
	}
	inline std::filesystem::path get_intermediate_dir() const {
		return project_file->parent_path() / build_cfg.intermediate_dir;
	}

	// build cache
	build_cache_file load_cache();
	void save_cache(build_cache_file& cache) const;
	std::vector<source_file> get_files_to_compile(build_cache_file& cache);

	source_metadata generate_src_metadata(const source_file& src);
	artifact_metadata generate_artifact_metadata(const code_artifact& art);

	code_artifact load_artifact_from_metadata(const artifact_metadata& meta);

	bool clang_build(const std::vector<std::string>& ir_files);

	inline std::string get_platform_app_extension() const;

	std::string replace_vars(const std::string& str);
};