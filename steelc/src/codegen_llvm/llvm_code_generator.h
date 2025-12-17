#pragma once

#include <codegen/icode_generator.h>
#include <codegen/codegen_result.h>

class llvm_code_generator : icode_generator {
	codegen_result emit(const mir_module& module) override;
};