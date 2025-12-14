#pragma once

#include <memory>

#include <representations/entities/entities_fwd.h>
#include <symbolics/symbol_table.h>

class entity_ref {
public:
	entity_ref()
		: id(-1) {
	}
	explicit entity_ref(entity_id eid)
		: id(eid) {
	}

	inline bool valid() const {
		return id >= 0;
	}
	inline entity_id get_id() const {
		return id;
	}

	entity_ptr resolve(const symbol_table& sym_table) const;

private:
	entity_id id;
};