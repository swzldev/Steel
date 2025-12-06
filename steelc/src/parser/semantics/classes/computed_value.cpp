#include "computed_value.h"

bool computed_value::operator==(const computed_value& other) const {
    return *type == other.type && value == other.value;
}
bool computed_value::operator!=(const computed_value& other) const {
    return !(*this == other);
}
bool computed_value::operator<(const computed_value& other) const {
    if (is_int() && other.is_int()) return as_int() < other.as_int();
    if (is_float() && other.is_float()) return as_float() < other.as_float();
    if (is_char() && other.is_char()) return as_char() < other.as_char();
    if (is_string() && other.is_string()) return value < other.value;
}
bool computed_value::operator<=(const computed_value& other) const {
    return *this < other || *this == other;
}
bool computed_value::operator>(const computed_value& other) const {
    return !(*this <= other);
}
bool computed_value::operator>=(const computed_value& other) const {
    return !(*this < other);
}

computed_value computed_value::operator+(const computed_value& other) {
	if (is_int() && other.is_int()) {
        return computed_value(to_data_type(DT_I32), std::to_string(as_int() + other.as_int()));
    }
    else if (is_float() && other.is_float()) {
        return computed_value(to_data_type(DT_FLOAT), std::to_string(as_float() + other.as_float()));
    }
    else if (is_string() && other.is_string()) {
        return computed_value(to_data_type(DT_STRING), as_string() + other.as_string());
	}
}
computed_value computed_value::operator-(const computed_value& other) {
	return computed_value(to_data_type(DT_I32), std::to_string(as_int() - other.as_int()));
}