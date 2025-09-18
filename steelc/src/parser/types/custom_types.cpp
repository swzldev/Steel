#include "custom_types.h"

#include "../ast/declarations/type_declaration.h"

std::shared_ptr<custom_type> custom_type::get(const std::string& identifier) {
	if (type_map.find(identifier) != type_map.end()) {
		return type_map[identifier];
	}
	auto new_type = std::shared_ptr<custom_type>(new custom_type(identifier));
	type_map[identifier] = new_type;
	return new_type;
}

int custom_type::size_of() const {
    int size = 0;
    if (declaration) {
        
    }
    return -1;
}

std::map<std::string, std::shared_ptr<custom_type>> custom_type::type_map;