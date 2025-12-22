#pragma once

#include <vector>

#include <codegen/codegen_config.h>
#include <codegen/code_artifact.h>

// icode_linker
//
// interface for intermediate code linkers
// 
// VERY IMPORTANT DETAIL:
// all (supported) IR artifacts will be passed to link()
// this means that the linker implementation MUST filter to
// avoid duplicate linking, for example, if using LLVM,
// artifacts may contain both LLVM-ASM and LLVM-BC formats
// the linker must choose one format to link from and ignore the others

class icode_linker {
public:
	virtual ~icode_linker() = default;

	virtual code_artifact link(const std::vector<code_artifact>& artifacts, const codegen_config& cfg) = 0;

	virtual bool supports(const code_artifact& artifact) = 0;
};