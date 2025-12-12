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
		// should change this to fit with to_full_name later
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

inline bool module_manager::has_module(const std::vector<std::string>& name_path) const {
	std::string full_name = module_path_to_full_name(name_path);
	for (const auto& mod : modules) {
		if (mod.first == full_name) {
			return true;
		}
	}
	return false;
}
std::shared_ptr<module_entity> module_manager::get_module(const std::vector<std::string>& name_path) {
	std::string full_name = module_path_to_full_name(name_path);
	for (const auto& mod : modules) {
		if (mod.first == full_name) {
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

std::string module_manager::module_path_to_full_name(const std::vector<std::string>& name_path) {
	// this function normalizes lookup paths into the actual path, this means we dont have to
	// deal with full name changes, e.g. parent::child may change to parent.child, etc, so
	// therefore we unify the format here
	std::string full_name;
	for (size_t i = 0; i < name_path.size(); i++) {
		full_name += name_path[i];
		if (i < name_path.size() - 1) {
			full_name += "::";
		}
	}
	return full_name;
}

std::shared_ptr<module_info> module_manager::create_info(const std::string& name) {
	static module_id next_id = 0;

	auto info = std::make_shared<module_info>();
	info->id = ++next_id;
	info->name = name;

	return info;
}
