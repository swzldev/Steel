#include "module_manager.h"

#include <string>
#include <memory>

#include <modules/module_info.h>
#include <representations/entities/entity.h>
#include <representations/entities/module_entity.h>

module_manager::module_manager() {
	auto info = create_info("<global module>");
	global_module = module_entity::make(info);
	// dont register as an actual module
	// we dont want to be able to look it up by name
}

std::shared_ptr<module_entity> module_manager::add_module(const std::string& name, std::shared_ptr<module_entity> parent) {
	// identify full name
	std::string full_name;
	if (parent && !parent->is_global()) {
		full_name = parent->full_name() + "::" + name;
	}
	else {
		full_name = name;
	}

	if (parent == nullptr) {
		parent = global_module;
	}

	// create module
	auto info = create_info(name);
	auto module = module_entity::make(info, parent);

	// register module
	modules[full_name] = module;

	return module;
}

inline bool module_manager::has_module(const std::string& name) const {
	for (const auto& mod : modules) {
		if (mod.second->name() == name) {
			return true;
		}
	}
	return false;
}
std::shared_ptr<module_entity> module_manager::get_module(const std::string& name) {
	for (const auto& mod : modules) {
		if (mod.second->name() == name) {
			return mod.second;
		}
	}
	return nullptr;
}
std::shared_ptr<module_entity> module_manager::get_global_module() {
	return global_module;
}
std::shared_ptr<module_entity> module_manager::get_parent(std::shared_ptr<module_entity> module) {
	return module->parent_module ? module->parent_module : global_module;
}

std::shared_ptr<module_info> module_manager::create_info(const std::string& name) {
	static module_id next_id = 0;

	auto info = std::make_shared<module_info>();
	info->id = ++next_id;
	info->name = name;

	return info;
}
