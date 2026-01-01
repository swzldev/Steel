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
#include <building/cache/vars_file.h>
#include <building/build_config.h>
#include <building/code_outputter.h>
#include <compiler/compiler.h>
#include <codegen/codegen.h>
#include <codegen/codegen_result.h>
#include <codegen/sys/system_formats.h>
#include <linking/linker.h>
#include <stproj/source_file.h>
#include <stproj/stproj_file.h>

static inline uint64_t now_unix_ms() {
	return static_cast<uint64_t>(
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count()
	);
}

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

	// validate config
	if (!validate_config()) {
		return false;
	}

	// load cache files
	build_cache_file cache = load_cache();
	load_vars_file();

	std::vector<source_file> to_compile;
	codegen_config codegen_cfg = generate_codegen_config();
	codegen_result codegen_result{};

	if (build_cfg.build_all) {
		to_compile = project_file->sources;
	}
	else {
		// due to only compiling specific files, the symbol table does not
		// receive all symbols (if theyre in files that arent being compiled)
		// until i fix this, ill temporarily force full builds
		to_compile = project_file->sources;
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

	// output code (if any)
	if (codegen_result.artifacts.size() > 0) {
		for (const auto& artifact : codegen_result.artifacts) {
			std::string path = get_artifact_path(artifact);

			code_output_error err = OUTPUT_SUCCESS;

			if (artifact.is_binary) {
				err = outputter->output_code(artifact.bytes, path);
			}
			else {
				err = outputter->output_code(artifact.text, path);
			}

			if (err != OUTPUT_SUCCESS) {
				output::err("Failed to output IR file: {}\n", console_colors::BOLD + console_colors::RED, path);
				return false;
			}
		}
	}

	std::string obj_format = system_formats::get_object_format();

	// gather all irs
	std::vector<std::string> to_link_paths;
	std::unordered_set<std::string> compiled_srcs;
	std::vector<std::string> missing;

	for (const auto& artifact : codegen_result.artifacts) {
		if (artifact.kind == ARTIFACT_OBJECT) {
			if (artifact.format != obj_format) {
				output::err("Error: generated object file has invalid format: {} (expected {})\n", console_colors::BOLD + console_colors::RED, artifact.format, obj_format);
				return false;
			}

			const std::string rel_art_path = get_artifact_path(artifact, true);
			const std::string art_path = get_artifact_path(artifact, false);

			// we need to keep track of which source files were compiled
			// so we can ensure we dont miss any
			compiled_srcs.insert(artifact.src_relpath);

			artifact_metadata art_meta = generate_artifact_metadata(artifact);
			cache.set_artifact_metadata(rel_art_path, art_meta);

			to_link_paths.push_back(art_path);
		}
	}
	for (const auto& src : project_file->sources) {
		if (compiled_srcs.contains(src.relative_path)) continue; // already compiled

		// lookup in cache
		std::optional<artifact_metadata> art_meta_opt = std::nullopt;
		for (const auto& [path, art_meta] : cache.get_artifact_metadata()) {
			if (art_meta.src_relpath == src.relative_path &&
				art_meta.kind == ARTIFACT_OBJECT &&
				art_meta.format == obj_format
			){
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
			to_link_paths.push_back(abs);
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
	
	output::print("Linking {} object file(s)...\n", console_colors::BOLD, to_link_paths.size());

	// link to executable
	link_data link_data{
		.object_files = to_link_paths,
		.object_format = obj_format,
		.cfg = generate_link_config(),
		.cached_vars = cached_vars
	};
	linker linker(link_data);
	if (!linker.linker_available()) {
		output::err("No suitable linker available for object format: {}\n", console_colors::BOLD + console_colors::RED, obj_format);
		return false;
	}

	link_result lnk_res = linker.link_all();
	if (!lnk_res.success) {
		output::err("Linking failed: {}\n", console_colors::BOLD + console_colors::RED, lnk_res.error.message);
		return false;
	}
	output::print("Linking succeeded.\n", console_colors::BOLD + console_colors::GREEN);

	output::print("Building succeeded. ", console_colors::BOLD + console_colors::GREEN);
	output::print("(Took {:.3f} seconds)\n", console_colors::DIM, get_build_time());

	// save cache(s)
	save_cache(cache);
	save_vars_file();

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

int project_builder::run_build_command(const std::string& command) {
	return system(command.c_str());
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
	case ARTIFACT_OBJECT: {
		path += build_cfg.intermediate_dir + "obj/";
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
	// DONT normalize relative paths
	return path;
}

build_cache_file project_builder::load_cache() {
	cache_path = path_utils::normalize(project_file->parent_path() / build_cfg.build_cache_file);
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

void project_builder::load_vars_file() {
	auto p = path_utils::normalize(project_file->parent_path() / build_cfg.vars_file);
	output::verbose("Loading vars file at: \'{}\'\n", console_colors::DIM, p.string());

	cached_vars.load_from_file(p);
}
void project_builder::save_vars_file() const {
	auto p = path_utils::normalize(project_file->parent_path() / build_cfg.vars_file);
	if (!cached_vars.save_to_file(p)) {
		output::print("Warn: Failed to save vars file.\n", console_colors::YELLOW);
	}
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
	meta.timestamp = now_unix_ms();
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

bool project_builder::validate_config() {
	// validate backend and format
	{
		std::string& backend = build_cfg.backend;
		std::string& ir_format = build_cfg.ir_format;

		if (!codegen::validate_backend(backend)) {
			output::err("Unknown codegen backend specified: {}\n", console_colors::BOLD + console_colors::RED, backend);
			return false;
		}
		if (ir_format.empty()) {
			ir_format = codegen::default_ir_format(backend);
			output::verbose("Using default backend format: \'{}\'\n", console_colors::DIM, ir_format);
		}
		if (!codegen::validate_ir_format(backend, ir_format)) {
			output::err("IR format \'{}\' is not supported by backend \'{}\'\n", console_colors::BOLD + console_colors::RED, ir_format, backend);
			return false;
		}
	}

	// validate target
	{

	}

	return true;
}

codegen_config project_builder::generate_codegen_config() const {
	return codegen_config{
		.backend = build_cfg.backend,
		.ir_format = build_cfg.ir_format,

		.target_triple = build_cfg.target_triple,
		.cpu = "",
		.features = {},

		.optimization_level = 2, // default optimization level

		.generate_debug_info = false,
	};
}
link_config project_builder::generate_link_config() const {
	return link_config{
		.output_dir = path_utils::normalize(get_output_dir()).string(),
		.output_name = project_filename(),
		.target = target_triple(build_cfg.target_triple),
	};
}
