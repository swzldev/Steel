#pragma once

#include <memory>
#include <unordered_map>

#include <symbolics/symbol_table.h>
#include <representations/entities/entities_fwd.h>

// allows for remapping entity_id's to different entities
// falling back to the base symbol_table when no overlay exists
class overlay_table : symbol_table {
public:
	overlay_table(const symbol_table& base_table)
		: symbol_table(base_table) {
	}

	void remap(entity_id id, entity_ptr entity);

private:
	std::unordered_map<entity_id, entity_ptr> overlay_entities;
};