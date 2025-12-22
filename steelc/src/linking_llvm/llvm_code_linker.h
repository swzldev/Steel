#pragma once

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <codegen/codegen_config.h>
#include <codegen/code_artifact.h>
#include <linking/icode_linker.h>

// llvm_code_linker
//
// this class implements an intermediate code linker for LLVM IR code

enum llvm_artifact_format {
	LLVM_ARTIFACT_FORMAT_UNKNOWN,
	LLVM_ARTIFACT_FORMAT_ASM,
	LLVM_ARTIFACT_FORMAT_BC,
};

class llvm_code_linker : public icode_linker {
public:
	virtual code_artifact link(const std::vector<code_artifact>& artifacts, const codegen_config& cfg) override;
	virtual bool supports(const code_artifact& artifact) override;

private:
	llvm::LLVMContext ctx;
	llvm_artifact_format art_format = LLVM_ARTIFACT_FORMAT_UNKNOWN;

	std::unique_ptr<llvm::Module> module_from_artifact(const code_artifact& a);
};