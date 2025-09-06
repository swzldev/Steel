#include "container_types.h"

bool array_type::operator==(const data_type& other) const {
    return other.primitive == DT_ARRAY && *base_type == other;
}

bool pointer_type::operator==(const data_type& other) const {
	return other.primitive == DT_POINTER && *base_type == other;
}
