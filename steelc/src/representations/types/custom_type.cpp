#include "custom_type.h"

#include <ast/declarations/type_declaration.h>

std::shared_ptr<custom_type> custom_type::get(const std::string& identifier, std::vector<type_ptr> generics) {
	if (generics.empty()) {
		if (type_map.count(identifier) > 0) {
			return type_map[identifier];
		}
	}
	else {
		if (generic_type_map.count(identifier) > 0) {
			for (const auto& gen_custom : generic_type_map[identifier]) {
				bool matches = true;
				for (size_t i = 0; i < generics.size(); i++) {
					if (gen_custom->generic_args.size() != generics.size()
						|| *gen_custom->generic_args[i] != generics[i]) {
						matches = false;
						break;
					}
				}
				if (matches) {
					return gen_custom;
				}
			}
		}
	}

	// bypass make_shared to access private constructor
	auto new_type = std::make_shared<custom_type>(identifier);
	if (!generics.empty()) {
		new_type->generic_args = generics;
		generic_type_map[identifier].push_back(new_type);
	}
	else {
		type_map[identifier] = new_type;
	}
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
std::map<std::string, std::vector<std::shared_ptr<custom_type>>> custom_type::generic_type_map;
