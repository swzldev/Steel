#include "enum_type.h"

#include <ast/declarations/enum_declaration.h>

bool enum_type::operator==(const type_ptr& other) const {
    if (auto other_enum = other->as_enum()) {
        // simple identifier check
		return identifier == other_enum->identifier;
    }
    return false;
}

int enum_type::size_of() const {
    if (declaration) {
        return declaration->base_type->size_of();
    }
    return -1;
}

std::shared_ptr<data_type> enum_type::clone() const {
    return std::make_shared<enum_type>(*this);
}
