#include "llvm_native_writer.h"

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>
#include <cstdint>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/LegacyPassManager.h>

using namespace llvm;

std::vector<uint8_t> llvm_native_writer::write_object() {
    // safe to recall
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    // default for empty configurations
    if (target_triple.empty()) {
		target_triple = sys::getDefaultTargetTriple();
    }
    if (cpu.empty()) {
        cpu = "generic";
	}

    // setup target
    Triple triple(target_triple);
    module->setTargetTriple(triple);

    std::string error;
    const Target* target = TargetRegistry::lookupTarget(triple, error);
    if (!target) {
        throw std::runtime_error(error);
    }

    TargetOptions opts;
    std::unique_ptr<TargetMachine> tm(
        target->createTargetMachine(
            triple,
            cpu,
            "", // no feature support for now
            opts,
            std::nullopt
        )
    );
    if (!tm) {
        throw std::runtime_error("Failed to create TargetMachine");
    }

    module->setDataLayout(tm->createDataLayout());

    // Emit object file to memory buffer
    SmallVector<char, 0> buffer;
    raw_svector_ostream os(buffer);

    legacy::PassManager pm;
    if (tm->addPassesToEmitFile(pm, os, nullptr, CodeGenFileType::ObjectFile)) {
        throw std::runtime_error("Target cannot emit object files");
    }

    pm.run(*module);

    return std::vector<uint8_t>(buffer.begin(), buffer.end());
}
