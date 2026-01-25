#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <llvm/IR/Module.h>

#include <codegen/code_artifact.h>
#include <sys/target_triple.h>

// llvm_native_writer
//
// utility class responsible for converting llvm modules to
// native binary/text data (e.g., object files, native assembly, etc.)

class llvm_native_writer {
public:
	llvm_native_writer(llvm::Module* module, const target_triple& target, const std::string& cpu)
		: module(module), target(target) {
	}

	std::vector<uint8_t> write_object();

private:
	llvm::Module* module;
	target_triple target;
	std::string cpu;
};