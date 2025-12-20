#pragma once

#include <linking/icode_linker.h>

// llvm_code_linker
//
// this class implements an intermediate code linker for LLVM IR code

class llvm_code_linker : public icode_linker {
public:
	virtual code_artifact link(const std::vector<code_artifact>& artifacts) override;
	virtual bool supports(const code_artifact& artifact) override;
};