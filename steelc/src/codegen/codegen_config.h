#pragma once

#include <string>
#include <unordered_map>

// codegen_config
//
// configuration options for code generation
// also includes choice of backend, e.g. LLVM

enum class codegen_backend {
	LLVM,
	CUSTOM,
};

class codegen_config {
public:
	codegen_config() = default;

	codegen_backend backend = codegen_backend::LLVM;

	// emission options (backend-specific)
	bool emit_llvm_bc = true;      // emit LLVM bitcode
	bool emit_llvm_asm = false;    // emit LLVM assembly

	// target options
	std::string target_triple = "";								// target triple (e.g. x86_64-pc-linux-gnu)
	std::string cpu = "";										// target CPU (e.g. x86-64)
	std::unordered_map<std::string, std::string> features;		// target features (e.g. +sse2, -avx)

	// optimization options
	int optimization_level = 0;		// optimization level (0-3)

	// debug options
	bool generate_debug_info = false;	// generate debug information
};