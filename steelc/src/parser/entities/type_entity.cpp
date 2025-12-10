#include "type_entity.h"

#include <string>
#include <memory>

#include "../types/types_fwd.h"
#include "../types/data_type.h"

std::shared_ptr<type_entity> type_entity::make(type_ptr type) {
	return std::shared_ptr<type_entity>(new type_entity(type));
}

std::string type_entity::name() const {
	return type->name();
}
std::string type_entity::full_name() const {
	// may change in the future when scoped
	// types are implemented e.g. module::type
	return type->name();
}

std::shared_ptr<type_entity> type_entity::as_type() {
	return shared_from_this();
}
