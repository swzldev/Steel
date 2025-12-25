#pragma once

#include <memory>
#include <vector>
#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Passes/OptimizationLevel.h>

#include <codegen/codegen_config.h>
#include <linking/link_result.h>
#include <linking/icode_linker.h>

// llvm_code_linker
//
// this class implements an intermediate code linker for LLVM IR code

using namespace llvm;

enum llvm_artifact_format {
	LLVM_ARTIFACT_FORMAT_UNKNOWN,
	LLVM_ARTIFACT_FORMAT_ASM,
	LLVM_ARTIFACT_FORMAT_BC,
};

class llvm_code_linker : public icode_linker {
public:
	virtual link_result link(const std::vector<code_artifact>& artifacts, const codegen_config& cfg) override;
	virtual bool supports(const code_artifact& artifact) override;

private:
	LLVMContext ctx;
	llvm_artifact_format art_format = LLVM_ARTIFACT_FORMAT_UNKNOWN;

	static void initialize_llvm_once();

	std::unique_ptr<Module> module_from_artifact(const code_artifact& a);

	OptimizationLevel get_optimization_level(int level);

	std::string get_object_format(Triple::OSType os);
	std::string get_object_extension(Triple::OSType os);
};