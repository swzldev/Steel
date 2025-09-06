#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../ast/ast_visitor.h"
#include "../compilation_pass.h"
#include "../modules/module_manager.h"

class import_resolver : public ast_visitor, public compilation_pass {
public:
	import_resolver(std::shared_ptr<class compilation_unit> unit, module_manager& module_manager)
		: unit(unit), module_manager(module_manager), compilation_pass(unit) {
	}

	void visit(std::shared_ptr<import_statement> import_stmt) override;

private:
	std::shared_ptr<class compilation_unit> unit;
	module_manager& module_manager;
};