#pragma once

#include <codegen/codegen_result.h>
#include <mir/mir_module.h>

// icode_generator
//
// this interface defines the contract for intermediate code generators
// that transform MIR modules into intermediate code representations

class icode_generator {
public:
	virtual ~icode_generator() = default;

	virtual codegen_result emit(const mir_module& module) = 0;
};
