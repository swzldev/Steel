#include "import_resolver.h"

#include "../ast/ast.h"

void import_resolver::visit(std::shared_ptr<class import_statement> import_stmt) {
	// find the module
	auto module = module_manager.get_module(import_stmt->module_name);
	if (module == nullptr) {
		ERROR(ERR_MODULE_NOT_FOUND, import_stmt->position, import_stmt->module_name.c_str());
		return;
	}

	// insert the module into the compilation units import table (file wide)
	unit->import_tbl.add_import(module);
}