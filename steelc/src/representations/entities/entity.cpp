#include "entity.h"

#include <memory>

#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity_ref.h>

entity_ptr entity::UNRESOLVED = std::make_shared<entity>(ENTITY_UNRESOLVED);

std::string entity::kind_string() const {
    switch (e_kind) {
    case ENTITY_UNRESOLVED:
			return "<unresolved>";

	case ENTITY_VARIABLE:
		return "variable";
	case ENTITY_FUNCTION:
		return "function";
	case ENTITY_TYPE:
		return "type";
	case ENTITY_MODULE:
		return "module";
	case ENTITY_GENERIC_PARAM:
		return "generic parameter";
    }
}

entity_ref entity::ref() const {
	return entity_ref(id);
}
