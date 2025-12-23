#include "llvm_code_linker.h"

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/AsmParser/Parser.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>

code_artifact llvm_code_linker::link(const std::vector<code_artifact>& artifacts, const codegen_config& cfg) {
	throw;
}
bool llvm_code_linker::supports(const code_artifact& artifact) {
	return false;
}

std::unique_ptr<llvm::Module> llvm_code_linker::module_from_artifact(const code_artifact& a) {
	if (a.format == "llvm-asm") {
		if (a.is_binary) throw std::runtime_error("Expected text data for llvm-asm artifact");

		llvm::SMDiagnostic err;
		auto mod = llvm::parseAssemblyString(a.text, err, ctx);
		if (!mod) {
			std::string err_msg;
			llvm::raw_string_ostream os(err_msg);
			err.print("llvm_code_linker", os);
			os.flush();
			throw std::runtime_error("Failed to parse LLVM assembly: " + err_msg);
		}
		return mod;
	}
	else if (a.format == "llvm-bc") {
		auto buf = llvm::MemoryBuffer::getMemBuffer(
			llvm::StringRef(reinterpret_cast<const char*>(a.bytes.data()), a.bytes.size()),
			"llvm_bc_artifact",
			false
		);
		auto mod_or_err = llvm::parseBitcodeFile(buf->getMemBufferRef(), ctx);
		if (!mod_or_err) {
			std::string err_msg;
			llvm::raw_string_ostream os(err_msg);
			llvm::handleAllErrors(mod_or_err.takeError(), [&](const llvm::ErrorInfoBase &EIB) {
				EIB.log(os);
			});
			os.flush();
			throw std::runtime_error("Failed to parse LLVM bitcode: " + err_msg);
		}
		return std::move(*mod_or_err);
	}
	else {
		throw std::runtime_error("Unsupported artifact format for LLVM module: " + a.format);
	}
}
