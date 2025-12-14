#include "entity_ref.h"

#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>

entity_ptr entity_ref::resolve(const symbol_table& sym_table) {
	if (!valid()) {
		return entity::UNRESOLVED;
	}
	last_entity = sym_table.get_by_id(id);
	return last_entity;
}
