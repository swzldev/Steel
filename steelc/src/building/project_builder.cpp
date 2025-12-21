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
#include <optional>

#include <llvm/IR/Module.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <config/compile_config.h>
#include <utils/console_colors.h>
#include <utils/path_utils.h>
#include <stproj/bad_stproj_exception.h>
#include <output/output.h>
#include <error/error_printer.h>
#include <building/cache/source_metadata.h>
#include <building/cache/artifact_metadata.h>
#include <building/cache/build_cache_file.h>
#include <building/cache/file_hasher.h>
#include <building/build_config.h>
#include <building/code_outputter.h>
#include <building/linking/link_error.h>
#include <building/linking/project_linker.h>
#include <compiler/compiler.h>
#include <codegen/codegen_result.h>
#include <stproj/source_file.h>
#include <stproj/stproj_file.h>

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

	// load build cache
	build_cache_file cache = load_cache();
	std::unordered_set<source_file> to_compile;
	codegen_config codegen_cfg{}; // TODO: generate config based on project settings + cl args
	codegen_result codegen_result{};

	if (build_cfg.build_all) {
		for (const auto& src : project_file->sources) {
			to_compile.insert(src);
		}
	}
	else {
		// due to only compiling specific files, the symbol table does not
		// receive all symbols (if theyre in files that arent being compiled)
		// until i fix this, ill temporarily force full builds
		for (const auto& src : project_file->sources) {
			to_compile.insert(src);
		}
		//to_compile = get_files_to_compile(cache);
	}

	// early out if nothing to compile
	if (to_compile.empty()) {
		output::print("No changes detected, skipping compilation.\n", console_colors::BLUE);
	}
	else {
		// compile sources
		compiler cmp = compiler(to_compile);
		compile_config compile_cfg{}; // TODO: generate config based on project settings + cl args

		mark_compile_start();
		output::print("Compiling {} source file(s)...\n", console_colors::BOLD, to_compile.size());
		if (cmp.compile(compile_cfg, codegen_cfg)) {
			output::print("Compilation succeeded. ", console_colors::BOLD + console_colors::GREEN);
			output::print("(Took {:.3f} seconds)\n", console_colors::DIM, get_compilation_time());
		}
		else {
			output::err("Compilation failed with {} errors.\n", console_colors::BOLD + console_colors::RED, cmp.get_error_count());
			error_printer::print_errors(cmp.get_errors());
			return false;
		}

		codegen_result = cmp.get_result();
	}

	output::verbose("Initializing output system...\n");
	output::verbose("Output directory: {}\n", console_colors::DIM, build_cfg.output_dir);
	output::verbose("Intermediate directory: {}\n", console_colors::DIM, build_cfg.intermediate_dir);
	outputter = code_outputter::create(project_file->parent_path().string(), build_cfg);
	if (!outputter) {
		output::err("Failed to initialize output system, ensure output paths are valid and accessable.\n", console_colors::BOLD + console_colors::RED);
		return false;
	}

	bool generate_asm = build_cfg.generate_llvm_asm;

	// output code (if any)
	if (codegen_result.artifacts.size() > 0) {
		// clear all irs
		outputter->clear_intermediate_files("IR");

		for (const auto& artifact : codegen_result.artifacts) {
			std::string path = get_artifact_path(artifact);

			code_output_error err = OUTPUT_SUCCESS;

			if (artifact.format == "LLVM-BC") {
				err = outputter->output_code(artifact.bytes, path, OUTPUT_LOCATION_INTERMEDIATE, generate_asm ? OUTPUT_FORMAT_TEXT : OUTPUT_FORMAT_BINARY);
			}
			else {
				output::err("Unknown artifact format: \"{}\"\n", console_colors::BOLD + console_colors::RED, artifact.format);
				return false;
			}

			if (err != OUTPUT_SUCCESS) {
				output::err("Failed to output IR file: {}\n", console_colors::BOLD + console_colors::RED, path);
				return false;
			}
		}
	}

	// gather all irs
	std::vector<code_artifact> to_link;
	std::unordered_set<std::string> compiled_srcs;
	std::unordered_set<std::string> ir_set; // non-relative paths
	std::vector<std::string> missing;

	for (const auto& artifact : codegen_result.artifacts) {
		if (artifact.kind == ARTIFACT_IR) {
			const std::string rel_out_path = get_artifact_path(artifact, false);
			const std::string out_path = get_artifact_path(artifact, false);

			ir_set.insert(out_path);
			// we need to keep track of which source files were compiled
			// so we can ensure we dont miss any
			compiled_srcs.insert(artifact.src_relpath);

			artifact_metadata art_meta = generate_artifact_metadata(artifact);
			cache.set_artifact_metadata(rel_out_path, art_meta);

			to_link.push_back(artifact);
		}
	}
	for (const auto& src : project_file->sources) {
		if (compiled_srcs.contains(src.relative_path)) continue; // already compiled

		// lookup in cache
		std::optional<artifact_metadata> art_meta_opt = std::nullopt;
		for (const auto& [path, art_meta] : cache.get_artifact_metadata()) {
			if (art_meta.src_relpath == src.relative_path && art_meta.kind == ARTIFACT_IR) {
				art_meta_opt = art_meta;
				break;
			}
		}
		if (!art_meta_opt.has_value()) {
			missing.push_back(src.relative_path);
			continue;
		}

		const std::string abs = path_utils::normalize(get_project_dir() / art_meta_opt.value().path).string();
		if (!std::filesystem::exists(abs)) {
			// remove from cache - file is missing
			cache.remove_artifact_metadata(art_meta_opt.value().src_relpath);
			missing.push_back(src.relative_path);
			continue;
		}
		else {
			ir_set.insert(abs);

			// we need to load the artifact data from disk
			// for linking
			code_artifact art = load_artifact_from_metadata(art_meta_opt.value());
			to_link.push_back(art);
		}
	}
	// report missing files
	if (!missing.empty()) {
		output::err("Error: missing IR artifacts for the following source files:\n", console_colors::BOLD + console_colors::RED);
		for (const auto& src : missing) {
			output::err(" - {}\n", console_colors::BOLD + console_colors::RED, src);
		}
		output::err("This is likely an internal error, it is reccomended to run a clean and rebuild to regenerate missing files.\n", console_colors::BOLD + console_colors::RED);
		return false;
	}

	// link to one module
	project_linker linker(to_link, codegen_cfg);
	if (linker.has_error()) {
		output::err("Link error: {}\n", console_colors::BOLD + console_colors::RED, linker.get_error_message());
		return false;
	}

	code_artifact linked = linker.link_all();
	if (linked.kind != ARTIFACT_BINARY || !linked.is_binary) {
		output::err("Linker produced invalid artifact kind.\n", console_colors::BOLD + console_colors::RED);
		return false;
	}

	std::string linked_filename = project_filename() + linked.extension;
	outputter->output_code(linked.bytes, linked_filename, OUTPUT_LOCATION_INTERMEDIATE, OUTPUT_FORMAT_BINARY);

	// build with clang
	/*if (!clang_build({ (outputter->get_intermediate_dir() / linked_filename).string()})) {
		output::err("Clang: building failed.\n", console_colors::BOLD + console_colors::RED);
		return false;
	}*/

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

	// save build cache
	save_cache(cache);

	return true;
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

std::string project_builder::get_artifact_path(const code_artifact& artifact, bool relative) const {
	std::string path = "./";
	switch (artifact.kind) {
	case ARTIFACT_IR: {
		path += build_cfg.intermediate_dir + "ir/";
		path += artifact.src_relpath;
		break;
	}
	case ARTIFACT_BINARY: {
		path += build_cfg.output_dir;
		path += project_file->filename_no_extension().string();
		break;
	}

	default:
		throw std::runtime_error("Unsupported or input-only code artifact kind.");
	}

	path += artifact.extension;

	if (!relative) {
		return path_utils::normalize(get_project_dir() / path).string();
	}
	return path_utils::normalize(path).string();
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
void project_builder::save_cache(build_cache_file& cache) const {
	if (cache.outdated()) {
		cache.upgrade();
	}
	cache.serialize(cache_path);
}
std::vector<source_file> project_builder::get_files_to_compile(build_cache_file& cache) {
	std::vector<source_file> out_files;

	// note: auto refreshes metadata in cache
	auto should_compile = [](const source_file& src, source_metadata& meta) {
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

	auto& metadata = cache.get_src_metadata();
	for (const auto& src : project_file->sources) {
		// check cache
		if (auto meta = metadata.find(src.full_path); meta != metadata.end()) {
			// found in cache - check if needs recompilation
			if (cache.outdated()) {
				// if the cache is outdated, recompile all files
				out_files.push_back(src);
			}
			else if (should_compile(src, meta->second)) {
				out_files.push_back(src);
			}
			continue;
		}

		// not in cache - compile and add
		out_files.push_back(src);
		cache.set_src_metadata(src.full_path, generate_src_metadata(src));
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
		cache.remove_src_metadata(path);
	}

	return out_files;
}

source_metadata project_builder::generate_src_metadata(const source_file& src) {
	source_metadata meta{};

	meta.path = src.full_path;
	meta.last_modified = src.get_last_modified_time();

	// use hasher for hash and size to avoid opening a new file handle
	meta.hash = file_hasher::hash_file(src.full_path, &meta.size);

	return meta;
}
artifact_metadata project_builder::generate_artifact_metadata(const code_artifact& art) {
	artifact_metadata meta{};

	meta.path = get_artifact_path(art, false);
	meta.kind = art.kind;
	meta.src_relpath = art.src_relpath;
	meta.name = art.name;
	meta.extension = art.extension;
	meta.format = art.format;

	return meta;
}

code_artifact project_builder::load_artifact_from_metadata(const artifact_metadata& meta) {
	code_artifact art{};
	art.kind = meta.kind;
	art.src_relpath = meta.src_relpath;
	art.name = meta.name;
	art.extension = meta.extension;
	art.format = meta.format;
	art.is_binary = meta.is_binary;
	art.attributes = meta.attributes;

	if (art.is_binary) {
		// load binary data
		std::string abs_path = path_utils::normalize(get_project_dir() / meta.path).string();
		std::ifstream file(abs_path, std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open artifact file: " + abs_path);
		}
		file.seekg(0, std::ios::end);
		size_t size = static_cast<size_t>(file.tellg());
		file.seekg(0, std::ios::beg);
		art.bytes.resize(size);
		file.read(reinterpret_cast<char*>(art.bytes.data()), size);
		file.close();
	}
	else {
		// load text data
		std::string abs_path = path_utils::normalize(get_project_dir() / meta.path).string();
		std::ifstream file(abs_path);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open artifact file: " + abs_path);
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		art.text = buffer.str();
		file.close();
	}
	return art;
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
