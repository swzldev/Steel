#include "module_entity.h"

#include <string>
#include <memory>

#include <modules/module_info.h>

std::string module_entity::name() const {
	return mod_info->name;
}
std::string module_entity::full_name() const {
	if (parent_module && !parent_module->is_global()) {
		return parent_module->full_name() + "::" + mod_info->name;
	}
	return mod_info->name;
}

std::shared_ptr<module_entity> module_entity::as_module() {
	return shared_from_this();
}

symbol_table& module_entity::symbols() const {
	return mod_info->symbols;
}
