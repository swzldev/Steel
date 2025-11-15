#pragma once

#include <memory>

#include "../parser/ast/ast.h"
#include "../parser/modules/module_manager.h"

class codegen {
public:
	codegen(module_manager& module_manager)
		: module_manager(module_manager) {
	}

private:
	module_manager& module_manager;
};