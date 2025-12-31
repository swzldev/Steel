#pragma once

#include <string>

class build_config {
public:
	// this constructor creates a default build configuration
	build_config() = default;

	// output options
	std::string output_dir = "build/";
	std::string intermediate_dir = "build/intermediate/";
	std::string build_cache_file = "build/cache/build_cache.dat";
	std::string target_triple = ""; // target triple (empty = default, throws if doesnt exist)
	std::string backend = "llvm"; // backend used (empty = llvm, throws if doesnt exist)
	std::string ir_format = ""; // ir format (empty = backend default, will throw error if unsupported)

	// build options
	bool build_all = false;
	bool no_link = false;
};