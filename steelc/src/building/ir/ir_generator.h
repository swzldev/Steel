#pragma once

#include <string>

#include <llvm/IR/Module.h>

// important:
// do not treat the strings as text, they may contain null bytes!
// maybe at some point i will change the return type to std::vector<uint8_t> or something

namespace ir_generator {
	// converts the module to llvm ir (.ll)
	std::string llvm_module_to_asm(llvm::Module& module);
	// converts the module to llvm bitcode (.bc)
	std::string llvm_module_to_bitcode(llvm::Module& module);
}