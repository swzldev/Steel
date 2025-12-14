#include "type_entity.h"

#include <string>
#include <memory>

#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <ast/declarations/type_declaration.h>

std::shared_ptr<type_entity> type_entity::make(std::shared_ptr<type_declaration> declaration) {
	static std::unordered_map<std::shared_ptr<type_declaration>, std::shared_ptr<type_entity>> cache;
	auto it = cache.find(declaration);
	if (it != cache.end()) {
		return it->second;
	}
	return std::shared_ptr<type_entity>(new type_entity(declaration));
}

std::string type_entity::name() const {
	return declaration->identifier;
}
std::string type_entity::full_name() const {
	// may change in the future when scoped
	// types are implemented e.g. module::type
	return declaration->identifier;
}

std::shared_ptr<type_entity> type_entity::as_type() {
	return shared_from_this();
}
