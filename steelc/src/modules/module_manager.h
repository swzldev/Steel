#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include <ast/ast_fwd.h>
#include <representations/entities/entities_fwd.h>

class module_info;

class module_manager {
public:
	module_manager();

	std::shared_ptr<module_entity> add_module(const std::string& name, std::shared_ptr<module_entity> parent);

	inline bool has_module(const std::vector<std::string>& name_path) const;
	std::shared_ptr<module_entity> get_module(const std::vector<std::string>& name_path);
	std::shared_ptr<module_entity> get_global_module();
	std::shared_ptr<module_entity> get_parent(std::shared_ptr<module_entity> module);

	static std::string module_path_to_full_name(const std::vector<std::string>& name_path);

	std::shared_ptr<function_declaration> entry_point;

private:
	std::shared_ptr<module_entity> global_module;
	std::unordered_map<std::string, std::shared_ptr<module_entity>> modules;

	std::shared_ptr<module_info> create_info(const std::string& name);
};