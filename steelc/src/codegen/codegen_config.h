#pragma once

#include <string>
#include <unordered_map>

#include <sys/target_triple.h>

// codegen_config
//
// configuration options for code generation
// also includes choice of backend, e.g. LLVM

struct codegen_config {
	// emission options
	std::string backend = "llvm"; // intermediate representation format (e.g. llvm-bc, llvm-asm)
	std::string ir_format = ""; // empty = backend default

	// target options
	target_triple target;
	std::string cpu = "generic";								// target CPU
	std::unordered_map<std::string, std::string> features;		// target features (e.g. +sse2, -avx)

	// optimization options
	int optimization_level = 0;		// optimization level (0-3)

	// debug options
	bool generate_debug_info = false;	// generate debug information
};