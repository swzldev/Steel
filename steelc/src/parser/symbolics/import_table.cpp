#include "import_table.h"

bool import_table::add_import(std::shared_ptr<module_entity> module) {
	if (imports.find(module) != imports.end()) {
		return false; // already imported
	}
	imports.insert(module);
	return true;
}
