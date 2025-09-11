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

private:
	module_manager& module_manager;
};