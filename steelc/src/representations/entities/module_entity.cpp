#include "module_entity.h"

#include <string>
#include <memory>
#include <vector>

#include <modules/module_info.h>

std::string module_entity::name() const {
	return mod_info->name;
}
std::string module_entity::full_name() const {
	// should change this - need to stop hardcoding the "::" separator
	if (parent_module && !parent_module->is_global()) {
		return parent_module->full_name() + "::" + mod_info->name;
	}
	return mod_info->name;
}

std::vector<std::string> module_entity::name_path() const {
	if (is_global()) {
		return {};
	}
	if (parent_module && !parent_module->is_global()) {
		auto path = parent_module->name_path();
		path.push_back(mod_info->name);
		return path;
	}
	return { mod_info->name };
}

std::shared_ptr<module_entity> module_entity::as_module() {
	return shared_from_this();
}

symbol_table& module_entity::symbols() const {
	return mod_info->symbols;
}
