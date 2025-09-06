#pragma once

#include <string>
#include <memory>
#include <unordered_set>

struct module_info;

class import_table {
public:
	import_table() = default;

	bool add_import(std::shared_ptr<module_info> module);
	inline bool has_import(const std::shared_ptr<module_info>& module) const {
		return imports.find(module) != imports.end();
	}
	inline const std::unordered_set<std::shared_ptr<module_info>>& get_imports() const {
		return imports;
	}
	
private:
	std::unordered_set<std::shared_ptr<module_info>> imports;
};