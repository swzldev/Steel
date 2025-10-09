#include "data_type.h"

#include "primitive_type.h"
#include "custom_type.h"
#include "container_types.h"

std::shared_ptr<data_type> data_type::UNKNOWN = std::make_shared<data_type>(DT_UNKNOWN);

inline bool data_type::operator!=(const data_type_ptr other) const {
	return !(*this == other);
}

inline type_ptr<primitive_type> data_type::as_primitive() const {
	if (!is_primitive()) return nullptr;
	return std::dynamic_pointer_cast<primitive_type>(shared_from_this());
}
inline type_ptr<custom_type> data_type::as_custom() const {
	if (!is_custom()) return nullptr;
	return std::dynamic_pointer_cast<custom_type>(shared_from_this());
}
inline type_ptr<array_type> data_type::as_array() const {
	if (!is_array()) return nullptr;
	return std::dynamic_pointer_cast<array_type>(shared_from_this());
}
inline type_ptr<pointer_type> data_type::as_pointer() const {
	if (!is_pointer()) return nullptr;
	return std::dynamic_pointer_cast<pointer_type>(shared_from_this());
}
inline type_ptr<generic_type> data_type::as_generic() const {
	if (!is_generic()) return nullptr;
	return std::dynamic_pointer_cast<generic_type>(shared_from_this());
}
