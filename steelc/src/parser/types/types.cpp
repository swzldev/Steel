#include "types.h"

std::shared_ptr<data_type> data_type::unknown = std::make_shared<data_type>(DT_UNKNOWN);

bool data_type::operator==(const data_type& other) const {
	if (primitive != other.primitive) {
		return false;
	}
	if (modifiers != other.modifiers) {
		return false;
	}
	return true;
}
inline bool data_type::operator!=(const data_type& other) const {
	return !(*this == other);
}

bool data_type::is_primitive() const {
	return true;
}
bool data_type::is_indexable() const {
	if (primitive == DT_STRING) {
		return true;
	}
	return false;
}

int data_type::size_of() const {
	return primitive_size_of(primitive);
}

std::string data_type::type_name() const {
	if (primitive == DT_UNKNOWN) {
		return "<Unknown Type>";
	}
	return to_string(primitive);
}
