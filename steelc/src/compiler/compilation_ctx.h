#pragma once

#include <modules/module_manager.h>
#include <compiler/instantiation_worklist.h>

struct compilation_ctx {
	explicit compilation_ctx(module_manager& mod_manager)
		: module_manager(mod_manager) {
	}

	module_manager& module_manager;
	instantiation_worklist inst_worklist;
};