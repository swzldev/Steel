#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "module_info.h"

class module_manager {
public:
	module_manager();

	std::shared_ptr<module_info> add_module(std::string& name, std::shared_ptr<module_info> parent);

	inline bool has_module(std::string& name) const;
	std::shared_ptr<module_info> get_module(std::string& name);
	std::shared_ptr<module_info> get_global_module();
	std::shared_ptr<module_info> get_parent(std::shared_ptr<module_info> module);

	std::shared_ptr<class function_declaration> main_declaration;

private:
	std::shared_ptr<module_info> global_module;
	std::unordered_map<std::string, std::shared_ptr<module_info>> modules;

	inline module_id get_next_id() const;
};