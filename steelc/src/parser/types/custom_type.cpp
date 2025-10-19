#include "custom_type.h"

#include "../ast/declarations/type_declaration.h"

std::shared_ptr<custom_type> custom_type::get(const std::string& identifier) {
	if (type_map.find(identifier) != type_map.end()) {
		return type_map[identifier];
	}
	// bypass make_shared to access private constructor
	auto new_type = std::shared_ptr<custom_type>(new custom_type(identifier));
	type_map[identifier] = new_type;
	return new_type;
}

bool custom_type::operator==(const type_ptr& other) const {
	if (auto custom = other->as_custom()) {
		// compare typename
		return identifier == custom->identifier;
	}
	return false;
}

int custom_type::size_of() const {
    int size = 0;
    if (declaration) {
        
    }
    return -1;
}

std::shared_ptr<data_type> custom_type::clone() const {
	return std::make_shared<custom_type>(*this);
}

std::map<std::string, std::shared_ptr<custom_type>> custom_type::type_map;