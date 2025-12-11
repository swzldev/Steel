#pragma once

#include <memory>
#include <unordered_set>

#include <representations/entities/entities_fwd.h>

class import_table {
public:
	import_table() = default;

	bool add_import(std::shared_ptr<module_entity> module);
	inline bool has_import(const std::shared_ptr<module_entity>& module) const {
		return imports.find(module) != imports.end();
	}
	inline const std::unordered_set<std::shared_ptr<module_entity>>& get_imports() const {
		return imports;
	}
	
private:
	std::unordered_set<std::shared_ptr<module_entity>> imports;
};