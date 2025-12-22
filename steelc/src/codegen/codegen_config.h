#pragma once

#include <string>
#include <unordered_map>

// codegen_config
//
// configuration options for code generation
// also includes choice of backend, e.g. LLVM

class codegen_config {
public:
	codegen_config() = default;

	// emission options
	std::string backend = "llvm"; // intermediate representation format (e.g. llvm-bc, llvm-asm)
	std::string ir_format = ""; // empty = backend default

	// target options
	std::string target_triple = "";								// target triple (e.g. x86_64-pc-linux-gnu)
	std::string cpu = "";										// target CPU (e.g. x86-64)
	std::unordered_map<std::string, std::string> features;		// target features (e.g. +sse2, -avx)

	// optimization options
	int optimization_level = 0;		// optimization level (0-3)

	// debug options
	bool generate_debug_info = false;	// generate debug information
};