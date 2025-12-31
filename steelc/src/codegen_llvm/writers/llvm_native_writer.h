#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <llvm/IR/Module.h>

#include <codegen/code_artifact.h>

// llvm_native_writer
//
// utility class responsible for converting llvm modules to
// native binary/text data (e.g., object files, native assembly, etc.)

class llvm_native_writer {
public:
	llvm_native_writer(llvm::Module* module, const std::string& target_tri, const std::string& cpu)
		: module(module), target_triple(target_tri), cpu(cpu) {
	}

	std::vector<uint8_t> write_object();

private:
	llvm::Module* module;
	std::string target_triple;
	std::string cpu;
};