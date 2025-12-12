#include "import_resolver.h"

#include <memory>

#include <ast/ast_node.h>
#include <ast/compilation_unit.h>
#include <ast/statements/import_statement.h>
#include <error/error_catalog.h>
#include <symbolics/import_table.h>

void import_resolver::visit(std::shared_ptr<import_statement> import_stmt) {
	// find the module
	auto module = module_manager.get_module(import_stmt->module_name);
	if (module == nullptr) {
		ERROR(ERR_MODULE_NOT_FOUND, import_stmt->position, import_stmt->module_name.c_str());
		return;
	}

	// insert the module into the compilation units import table (file wide)
	unit->import_tbl.add_import(module);
}