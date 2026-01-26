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
	void lower_functions(const std::vector<ast_ptr>& decls, mir_module& mm);

	mir_function lower_func(std::shared_ptr<function_declaration> func);
};