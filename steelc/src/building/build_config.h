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
	bool generate_llvm_asm = false; // generate (llvm) asm instead of bitcode

	// build options
	bool build_all = false;
	bool no_link = false;
};