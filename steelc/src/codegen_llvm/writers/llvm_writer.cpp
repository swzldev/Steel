#include "llvm_writer.h"

#include <string>
#include <vector>
#include <cstdint>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/FileSystem.h>

#include <codegen/sys/system_formats.h>

std::vector<uint8_t> llvm_writer::write_bitcode() const {
	std::string buffer;
	llvm::raw_string_ostream rso(buffer);
	llvm::WriteBitcodeToFile(*module, rso);
	rso.flush();
	return std::vector<uint8_t>(buffer.begin(), buffer.end());
}
std::string llvm_writer::write_asm() const {
	std::string buffer;
	llvm::raw_string_ostream rso(buffer);
	module->print(rso, nullptr);
	rso.flush();
	return buffer;
}
