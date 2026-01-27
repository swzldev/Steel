#pragma once

#include <memory>
#include <vector>

#include <ast/ast_fwd.h>
#include <mir/mir_fwd.h>
#include <compiler/compilation_ctx.h>

// mir_lowerer
// this class is responsible for lowering the AST to MIR

class mir_lowerer {
public:
	explicit mir_lowerer(compilation_ctx& ctx)
		: ctx(ctx) {
	}

	std::vector<mir_module> lower_all(const std::vector<std::shared_ptr<compilation_unit>>& units);

private:
	compilation_ctx& ctx;
	std::unordered_map<unsigned int, mir_module> lowered;

	mir_module& get_module(const std::shared_ptr<compilation_unit>& unit);

	mir_module lower_unit(std::shared_ptr<compilation_unit> unit);
	void lower_functions(const std::vector<ast_ptr>& decls, mir_module& mm);

	mir_function lower_func(std::shared_ptr<function_declaration> func);
};