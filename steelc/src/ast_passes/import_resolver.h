#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

#include <ast/ast_visitor.h>
#include <ast/ast_fwd.h>
#include <compiler/compilation_pass.h>
#include <compiler/compilation_ctx.h>
#include <modules/module_manager.h>

class import_resolver : public ast_visitor, public compilation_pass {
public:
	import_resolver(std::shared_ptr<compilation_unit> unit, compilation_ctx& ctx)
		: unit(unit), module_manager(ctx.module_manager), compilation_pass(unit) {
	}

	void visit(std::shared_ptr<import_statement> import_stmt) override;

private:
	std::shared_ptr<compilation_unit> unit;
	module_manager& module_manager;
};