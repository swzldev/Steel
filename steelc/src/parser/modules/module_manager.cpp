#include "module_manager.h"

#include <algorithm>

module_manager::module_manager() {
	global_module = std::make_shared<module_info>();
	global_module->id = get_next_id();
	global_module->name = "<GLOBAL MODULE>";
	global_module->parent_module = nullptr;
}

std::shared_ptr<module_info> module_manager::add_module(std::string& name, std::shared_ptr<module_info> parent) {
	auto module = std::make_shared<module_info>();
	module->id = get_next_id();
	module->name = name;
	module->parent_module = parent;
	modules[name] = module;
	return module;
}

inline bool module_manager::has_module(std::string& name) const {
	for (const auto& mod : modules) {
		if (mod.second->name == name) {
			return true;
		}
	}
	return false;
}
std::shared_ptr<module_info> module_manager::get_module(std::string& name) {
	for (const auto& mod : modules) {
		if (mod.second->name == name) {
			return mod.second;
		}
	}
	return nullptr;
}
std::shared_ptr<module_info> module_manager::get_global_module() {
	return global_module;
}
std::shared_ptr<module_info> module_manager::get_parent(std::shared_ptr<module_info> module) {
	return module->parent_module ? module->parent_module : global_module;
}

inline module_id module_manager::get_next_id() const {
	static module_id next_id = 0;
	return next_id++;
}