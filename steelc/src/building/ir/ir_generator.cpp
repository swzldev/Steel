#include "ir_generator.h"

#include <string>

#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>

std::string ir_generator::llvm_module_to_asm(llvm::Module& module) {
	std::string buffer;
	llvm::raw_string_ostream stream(buffer);
	module.print(stream, nullptr);
	stream.flush();
	return buffer;
}
std::string ir_generator::llvm_module_to_bitcode(llvm::Module& module) {
	std::string buffer;
	llvm::raw_string_ostream stream(buffer);
	llvm::WriteBitcodeToFile(module, stream);
	stream.flush();
	return buffer;
}