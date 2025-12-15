#include "enum_entity.h"

#include <string>
#include <memory>
#include <unordered_map>

#include <ast/declarations/enum_declaration.h>

std::shared_ptr<enum_entity> enum_entity::get(std::shared_ptr<enum_declaration> declaration) {
	static std::unordered_map<std::shared_ptr<enum_declaration>, std::shared_ptr<enum_entity>> cache;
	auto it = cache.find(declaration);
	if (it != cache.end()) {
		return it->second;
	}
	return std::shared_ptr<enum_entity>(new enum_entity(declaration));
}

std::string enum_entity::name() const {
	return declaration->identifier;
}
std::string enum_entity::full_name() const {
	// may change in the future when scoped
	// types are implemented e.g. module::type
	return declaration->identifier;
}

std::shared_ptr<enum_entity> enum_entity::as_enum() {
	return shared_from_this();
}
