#include "entity_ref.h"

entity_ptr entity_ref::get(const std::shared_ptr<symbol_table>& sym_table) const {
	return sym_table->get_by_id(id);
}
