#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "module_info.h"

class module_manager {
public:
	module_manager();

	std::shared_ptr<module_info> add_module(const std::string& name, std::shared_ptr<module_info> parent);

	inline bool has_module(const std::string& name) const;
	std::shared_ptr<module_info> get_module(const std::string& name);
	std::shared_ptr<module_info> get_global_module();
	std::shared_ptr<module_info> get_parent(std::shared_ptr<module_info> module);

	std::shared_ptr<class function_declaration> entry_point;

private:
	std::shared_ptr<module_info> global_module;
	std::unordered_map<std::string, std::shared_ptr<module_info>> modules;

	inline module_id get_next_id() const;
};