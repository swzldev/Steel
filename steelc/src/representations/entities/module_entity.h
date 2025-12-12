#pragma once

#include <memory>
#include <string>
#include <vector>

#include <representations/entities/entity.h>

struct module_info;
class symbol_table;

class module_entity : public entity, public std::enable_shared_from_this<module_entity> {
public:
	static std::shared_ptr<module_entity> make(std::shared_ptr<module_info> mod_info, std::shared_ptr<module_entity> owner = nullptr) {
		return std::shared_ptr<module_entity>(new module_entity(mod_info, owner));
	}

	std::string name() const override;
	std::string full_name() const override;

	std::vector<std::string> name_path() const;

	std::shared_ptr<module_entity> as_module() override;

	// shorthands
	symbol_table& symbols() const;

	inline bool is_global() const {
		return parent_module == nullptr;
	}

	std::shared_ptr<module_info> mod_info;
	std::shared_ptr<module_entity> parent_module = nullptr;

private:
	module_entity(std::shared_ptr<module_info> mod_info, std::shared_ptr<module_entity> owner)
		: entity(ENTITY_MODULE), mod_info(mod_info), parent_module(owner) {
	}
};