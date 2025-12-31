#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <llvm/IR/Module.h>

#include <codegen/code_artifact.h>

// llvm_writer
//
// utility class responsible for converting llvm modules to
// binary/text data (and maybe more in the future)

class llvm_writer {
public:
	llvm_writer(llvm::Module* module)
		: module(module) {
	}

	std::vector<uint8_t> write_bitcode() const;
	std::string write_asm() const;

private:
	llvm::Module* module;
};