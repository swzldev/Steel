#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../parser/ast/ast.h"
#include "../parser/modules/module_manager.h"

class interpreter {
public:
	interpreter(module_manager& module_manager)
		: module_manager(module_manager) {
	}

	void begin_execution();
	void add_builtin_function(const std::string& identifier, type_ptr return_type, const std::vector<std::shared_ptr<variable_declaration>>& parameters);

private:
	module_manager& module_manager;
};