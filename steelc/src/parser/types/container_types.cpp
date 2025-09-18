#include "container_types.h"

bool array_type::operator==(const data_type& other) const {
	if (auto other_arr = dynamic_cast<const array_type*>(&other)) {
		return *base_type == *(other_arr->base_type);
	}
	return false;
}

bool pointer_type::operator==(const data_type& other) const {
	if (auto other_ptr = dynamic_cast<const pointer_type*>(&other)) {
		return *base_type == *(other_ptr->base_type);
	}
	return false;
}

bool generic_type::operator==(const data_type& other) const {
	if (auto other_gen = dynamic_cast<const generic_type*>(&other)) {
		if (substitution && other_gen->substitution) {
			return *substitution == *(other_gen->substitution);
		}
	}
	return false;
}
