#include "custom_type.h"

#include "../ast/declarations/type_declaration.h"

std::shared_ptr<custom_type> custom_type::get(const std::string& identifier) {
	if (type_map.find(identifier) != type_map.end()) {
		return type_map[identifier];
	}
	auto new_type = std::shared_ptr<custom_type>(new custom_type(identifier));
	type_map[identifier] = new_type;
	return new_type;
}

bool custom_type::operator==(const data_type_ptr other) const {
	if (!other->is_custom()) return false;
	return name() == other->as_custom()->name();
}

int custom_type::size_of() const {
    if (declaration) {
		int size = 0;
		for (const auto& field : declaration->fields) {
			size += field->type->size_of();
		}
		return size;
    }
    return -1;
}

std::map<std::string, std::shared_ptr<custom_type>> custom_type::type_map;