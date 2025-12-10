#include "data_type.h"

#include <memory>
#include <string>

#include "custom_type.h"
#include "container_types.h"
#include "enum_type.h"
#include "function_type.h"
#include "type_utils.h"

std::shared_ptr<data_type> data_type::UNKNOWN = std::make_shared<data_type>(DT_UNKNOWN);

bool data_type::operator==(const type_ptr& other) const {
	if (primitive != other->primitive) {
		return false;
	}
	if (modifiers != other->modifiers) {
		return false;
	}
	return true;
}
bool data_type::operator!=(const type_ptr& other) const {
	return !(*this == other);
}

std::shared_ptr<custom_type> data_type::as_custom() {
	if (is_custom()) {
		return std::dynamic_pointer_cast<custom_type>(shared_from_this());
	}
	return nullptr;
}
std::shared_ptr<array_type> data_type::as_array() {
	if (is_array()) {
		return std::dynamic_pointer_cast<array_type>(shared_from_this());
	}
	return nullptr;
}
std::shared_ptr<pointer_type> data_type::as_pointer() {
	if (is_pointer()) {
		return std::dynamic_pointer_cast<pointer_type>(shared_from_this());
	}
	return nullptr;
}
std::shared_ptr<data_type> data_type::as_reference() {
	// TEMPORARY
	return nullptr;
}
std::shared_ptr<enum_type> data_type::as_enum() {
	if (is_enum()) {
		return std::dynamic_pointer_cast<enum_type>(shared_from_this());
	}
	return nullptr;	
}
std::shared_ptr<generic_type> data_type::as_generic() {
	if (is_generic()) {
		return std::dynamic_pointer_cast<generic_type>(shared_from_this());
	}
	return nullptr;
}
std::shared_ptr<function_type> data_type::as_function() {
	if (is_function()) {
		return std::dynamic_pointer_cast<function_type>(shared_from_this());
	}
	return nullptr;
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
	return size_of_primitive(primitive);
}

std::string data_type::name() const {
	if (primitive == DT_UNKNOWN) {
		return "<Unknown Type>";
	}
	return to_string(primitive);
}

std::shared_ptr<data_type> data_type::clone() const {
	return std::make_shared<data_type>(*this);
}
