#pragma once

#include <memory>

#include <ast/ast_fwd.h>
#include <mir/mir_fwd.h>

// mir_lowerer
// this class is responsible for lowering the AST to MIR

class mir_lowerer {
public:
	mir_module lower_unit(std::shared_ptr<compilation_unit> unit);

private:
	mir_function lower_func(std::shared_ptr<function_declaration> func);
};