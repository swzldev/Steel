#include "overlay_table.h"

void overlay_table::remap(entity_id id, entity_ptr entity) {
	overlay_entities[id] = entity;
}

entity_ptr overlay_table::get_by_id(entity_id id) const {
	auto it = overlay_entities.find(id);
	if (it != overlay_entities.end()) {
		return it->second;
	}
	return symbol_table::get_by_id(id);
}
