#pragma once

#include <memory>

#include <representations/entities/entities_fwd.h>
#include <symbolics/symbol_table.h>

class entity_ref {
public:
	entity_ref()
		: id(-1) {
	}

	entity_ptr get(const std::shared_ptr<symbol_table>& sym_table) const;

private:
	entity_id id;
};