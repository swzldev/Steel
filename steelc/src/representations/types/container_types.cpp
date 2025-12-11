#include "container_types.h"

bool array_type::operator==(const type_ptr& other) const {
	if (auto other_arr = other->as_array()) {
		return *base_type == other_arr->base_type;
	}
	return false;
}

bool pointer_type::operator==(const type_ptr& other) const {
	if (auto other_ptr = other->as_pointer()) {
		return *base_type == other_ptr->base_type;
	}
	return false;
}

bool generic_type::operator==(const type_ptr& other) const {
	// this always returns false as no comparison between
	// two generic types should never occur
	return false;
}
