#pragma once

#include <memory>

#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <symbolics/symbol_table.h>

class entity_ref {
public:
	entity_ref()
		: id(-1) {
	}
	explicit entity_ref(entity_id eid)
		: id(eid) {
	}
	explicit entity_ref(entity_ptr ent)
		: id(ent->get_id()), last_entity(ent) {
	}

	inline bool valid() const {
		return id >= 0;
	}
	inline entity_id get_id() const {
		return id;
	}

	// returns the last resolved entity
	// may be UNRESOLVED if it has not been resolved yet
	// avoids needing to pass current symbol table
	// but may be stale
	inline entity_ptr get() const {
		return last_entity;
	}
	entity_ptr resolve(const symbol_table& sym_table);

private:
	entity_id id;
	entity_ptr last_entity = entity::UNRESOLVED;
};