#pragma once

#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <mir/mir_module.h>

// icode_generator
//
// this interface defines the contract for intermediate code generators
// that transform MIR modules into intermediate code representations

class icode_generator {
public:
	virtual ~icode_generator() = default;

	// generate a codegen result from a mir_module
	virtual codegen_result emit(const mir_module& module, const codegen_config& cfg) = 0;
};
