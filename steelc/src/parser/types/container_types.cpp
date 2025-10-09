#include "container_types.h"

#include "../ast/generics/generic_parameter.h"

bool array_type::operator==(const data_type_ptr other) const {
	if (auto other_arr = other->as_array()) {
		return base_type == other_arr->base_type;
	}
	return false;
}

bool pointer_type::operator==(const data_type_ptr other) const {
	if (auto other_ptr = other->as_pointer()) {
		return base_type == other_ptr->base_type;
	}
	return false;
}

bool generic_type::operator==(const data_type_ptr other) const {
	return declaration && declaration->constraint && declaration->constraint == other;
}

std::string generic_type::name() const {
	if (declaration && declaration->constraint) {
		return declaration->constraint->name();
	}
	return identifier;
}
