#include "project_builder.h"

#include <string>
#include <chrono>
#include <cstdlib>
#include <regex>
#include <vector>
#include <filesystem>
#include <memory>
#include <cstdint>
#include <unordered_set>

#include "cache/file_metadata.h"
#include "cache/build_cache_file.h"
#include "cache/file_hasher.h"
#include "build_config.h"
#include "code_outputter.h"
#include "../compiler.h"
#include "../config/compile_config.h"
#include "../utils/console_colors.h"
#include "../stproj/source_file.h"
#include "../stproj/stproj_file.h"
#include "../stproj/bad_stproj_exception.h"
#include "../output/output.h"
#include "../error/error_printer.h"
#include "../codegen/ir/ir_holder.h"

bool project_builder::load_project(const std::string& project_path) {
	// load stproj
	output::verbose("Loading project file at: \'{}\'\n", console_colors::DIM, project_path);
	try {
		project_file = stproj_file::load(project_path);
	}
	catch (const bad_stproj_exception& err) {
		output::err("Error loading project file: ", console_colors::RED);
		output::err(err.message() + "\n");
		return false;
	}
}
bool project_builder::build_project() {
	mark_build_start();
	output::print("Build started...\n");

	// compile project
	std::vector<ir_holder> generated_ir = compile_project();

	// output code (if any)
	if (generated_ir.size() > 0) {
		output::verbose("Initializing output system...\n");
		output::verbose("Output directory: {}\n", console_colors::DIM, build_cfg.output_dir);
		output::verbose("Intermediate directory: {}\n", console_colors::DIM, build_cfg.intermediate_dir);
		outputter = code_outputter::create(project_file->parent_path().string(), build_cfg);
		if (!outputter) {
			output::err("Failed to initialize output system, ensure output paths are valid and accessable.\n", console_colors::BOLD + console_colors::RED);
			return false;
		}

		for (const auto& il_h : generated_ir) {
			std::string path = get_ir_path(*il_h.owning_unit->source_file);
			if (outputter->output_il(il_h.ir, path) != OUTPUT_SUCCESS) {
				output::err("Failed to output IR file: {}\n", console_colors::BOLD + console_colors::RED, path);
				return false;
			}
		}
	}

	// gather all irs
	for (auto& src : project_file->sources) {
		all_irs.push_back(get_ir_path(src, false));
	}

	// build with clang
	if (!clang_build(all_irs)) {
		output::err("Clang: building failed.\n", console_colors::BOLD + console_colors::RED);
		return false;
	}

	output::print("Building succeeded. ", console_colors::BOLD + console_colors::GREEN);
	output::print("(Took {:.3f} seconds)\n", console_colors::DIM, get_build_time());

	// run post-build commands
	for (const auto& cmd : post_build_commands) {
		output::print("Running post-build command: {}\n", console_colors::BOLD + console_colors::CYAN, cmd);
		if (int ec = run_build_command(cmd); ec != 0) {
			output::err("Post-build command \"{}\" failed with exit code: {}\n", console_colors::BOLD + console_colors::RED, cmd, ec);
			return false;
		}
	}

	return true;
}
std::vector<ir_holder> project_builder::compile_project() {
	// load build cache
	build_cache_file cache = load_cache();
	std::vector<source_file> to_compile;

	if (build_cfg.build_all) {
		to_compile = project_file->sources;
	}
	else {
		to_compile = get_files_to_compile(cache);
	}

	// early out if nothing to compile
	if (to_compile.empty()) {
		output::print("No changes detected, skipping compilation.\n", console_colors::BLUE);
		return {};
	}

	// compile sources
	compiler cmp = compiler(to_compile);
	compile_config compile_cfg{}; // TODO: generate config based on project settings + cl args

	mark_compile_start();
	output::print("Compiling {} source file(s)...\n", console_colors::BOLD, to_compile.size());
	if (cmp.compile(compile_cfg)) {
		output::print("Compilation succeeded. ", console_colors::BOLD + console_colors::GREEN);
		output::print("(Took {:.3f} seconds)\n", console_colors::DIM, get_compilation_time());

		// save build cache
		save_cache(cache);
	}
	else {
		output::err("Compilation failed with {} errors.\n", console_colors::BOLD + console_colors::RED, cmp.get_error_count());
		error_printer::print_errors(cmp.get_errors());
		return {};
	}

	return cmp.get_generated_ir();
}

int project_builder::run_build_command(const std::string& command) {
	std::string replaced = replace_vars(command);
	return system(replaced.c_str());
}

double project_builder::get_build_time() const {
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - build_start).count();
}
double project_builder::get_compilation_time() const {
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - compile_start).count();
}

std::string project_builder::get_ir_path(const source_file& src, bool relative) const {
	std::string rel_path = "./IL/" + src.relative_path + ".ll";
	if (!relative) {
		return (get_intermediate_dir() / rel_path).string();
	}
	return rel_path;
}

build_cache_file project_builder::load_cache() {
	cache_path = project_file->parent_path() / build_cfg.build_cache_file;
	output::verbose("Loading build cache at: \'{}\'\n", console_colors::DIM, cache_path.string());
	// load existing cache
	if (std::filesystem::exists(cache_path)) {
		build_cache_file cache;
		if (cache.deserialize(cache_path)) {
			return cache;
		}
		else {
			output::print("Warn: failed to load build cache, using default.\n", console_colors::YELLOW);
		}
	}

	// create a new blank cache
	return build_cache_file{};
}
void project_builder::save_cache(const build_cache_file& cache) const {
	cache.serialize(cache_path);
}
std::vector<source_file> project_builder::get_files_to_compile(build_cache_file& cache) {
	std::vector<source_file> out_files;

	// note: auto refreshes metadata in cache
	auto should_compile = [](const source_file& src, file_metadata& meta) {
		uint64_t current_last_modified = src.get_last_modified_time();
		uint64_t current_size = src.get_size();
		if (current_last_modified == meta.last_modified &&
			current_size == meta.size)
		{
			return false;
		}

		uint64_t current_hash = file_hasher::hash_file(src.full_path);

		bool changed = (current_hash != meta.hash);

		// update metadata AFTER detecting change
		meta.last_modified = current_last_modified;
		meta.size = current_size;
		meta.hash = current_hash;

		return changed;
	};

	auto& metadata = cache.get_metadata();
	for (const auto& src : project_file->sources) {
		// check cache
		if (auto meta = metadata.find(src.full_path); meta != metadata.end()) {
			// found in cache - check if needs recompilation
			if (should_compile(src, meta->second)) {
				out_files.push_back(src);
			}
			continue;
		}

		// not in cache - compile and add
		out_files.push_back(src);
		cache.set_metadata(src.full_path, generate_metadata(src));
	}

	// cleanup cache - remove entries for files that no longer exist
	std::unordered_set<std::string> source_set;
	for (const auto& src : project_file->sources) {
		source_set.insert(src.full_path);
	}

	std::vector<std::string> to_remove;
	for (const auto& [path, meta] : metadata) {
		if (!source_set.contains(path)) {
			to_remove.push_back(path);
		}
	}
	for (const auto& path : to_remove) {
		cache.remove_metadata(path);
	}

	return out_files;
}
file_metadata project_builder::generate_metadata(const source_file& src) {
	file_metadata meta{};

	meta.path = src.full_path;
	meta.last_modified = src.get_last_modified_time();

	// use hasher for hash and size to avoid opening a new file handle
	meta.hash = file_hasher::hash_file(src.full_path, &meta.size);

	return meta;
}

bool project_builder::clang_build(const std::vector<std::string>& ir_files) {
	std::string cmd = "clang ";
	for (const auto& ir : ir_files) {
		cmd += "\"" + ir + "\" ";
	}
	cmd += "-o \"" + (get_output_dir() / (project_filename() + get_platform_app_extension())).string() + "\"";
	return system(cmd.c_str()) == 0;
}

inline std::string project_builder::get_platform_app_extension() const {
#if defined(_WIN32) || defined(_WIN64)
	return ".exe";
#elif defined(__APPLE__) || defined(__linux__)
	return ".out";
#else
	return "";
#endif
}

std::string project_builder::replace_vars(const std::string& str) {
	std::string remaining = str;
	std::string out;
	std::regex var_re(R"(\$\{([^}]+)\})");
	std::smatch m;

	while (std::regex_search(remaining, m, var_re)) {
		out += m.prefix().str();

		std::string key = m[1].str();
		if (key == "IRS") {
			for (size_t i = 0; i < all_irs.size(); ++i) {
				if (i) out.push_back(' ');
				out += "\"" + all_irs[i] + "\"";
			}
		}
		else if (key == "OUTPUT_DIR") {
			out += get_output_dir().string();
		}
		
		remaining = m.suffix().str();
	}

	out += remaining;
	return out;
}
