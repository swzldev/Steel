#include "project_linker.h"

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Error.h>
#include <llvm/Linker/Linker.h>

bool project_linker::load_modules_from_paths(const std::vector<std::string>& paths, bool is_bitcode) {
	modules.clear();

	llvm::LLVMContext ctx;
	llvm::SMDiagnostic err;

	// load modules from paths
	for (const auto& path : paths) {
		// load as bitcode
		if (is_bitcode) {
			auto buffer_or_err = llvm::MemoryBuffer::getFile(path);
			if (!buffer_or_err) {
				error(LINK_FAIL_LOAD_IR, "Failed to load IR file: " + buffer_or_err.getError().message());
				return false;
			}

			std::unique_ptr<llvm::MemoryBuffer> buffer = std::move(buffer_or_err.get());

			auto module_or_err = llvm::parseBitcodeFile(buffer->getMemBufferRef(), ctx);
			if (!module_or_err) {
				error(LINK_FAIL_LOAD_IR, "Failed to parse bitcode file: " + llvm::toString(module_or_err.takeError()));
				return false;
			}

			modules.push_back(std::move(module_or_err.get()));
			continue;
		}

		// load as LLVM IR
		std::unique_ptr<llvm::Module> mod = llvm::parseIRFile(path, err, ctx);

		if (!mod) {
			error(LINK_FAIL_LOAD_IR, "Failed to load llvm IR file at: " + path);
			return false;
		}

		modules.push_back(std::move(mod));
	}

	return true;
}

std::unique_ptr<llvm::Module> project_linker::link_all() {
	if (modules.empty()) {
		error(LINK_FAIL_NO_IR, "No IR modules to link");
		return nullptr;
	}

	llvm::Linker linker(*modules[0]);

	for (size_t i = 1; i < modules.size(); ++i) {
		if (linker.linkInModule(std::move(modules[i]))) {
			error(LINK_FAIL_LINK_MODULE, "Failed to link module (index " + std::to_string(i) + ")");
			return nullptr;
		}
	}

	return std::move(modules[0]);
}
