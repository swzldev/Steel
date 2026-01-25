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

#include <sys/target_triple.h>

using namespace llvm;

bool llvm_native_writer::write_object(std::vector<uint8_t>& output_buffer) {
    // safe to recall
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    // setup target
    Triple triple(target.stringify());
    module->setTargetTriple(triple);

    std::string err;
    const Target* target = TargetRegistry::lookupTarget(triple, error);
    if (!target) {
        error = err;
        return false;
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
		error = "Could not create target machine";
        return false;
    }

    module->setDataLayout(tm->createDataLayout());

    // Emit object file to memory buffer
    SmallVector<char, 0> buffer;
    raw_svector_ostream os(buffer);

    legacy::PassManager pm;
    if (tm->addPassesToEmitFile(pm, os, nullptr, CodeGenFileType::ObjectFile)) {
		error = "TargetMachine can't emit object file";
		return false;
    }

    pm.run(*module);

    output_buffer.assign(buffer.begin(), buffer.end());
    return true;
}
