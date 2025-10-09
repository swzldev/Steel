#include "primitive_type.h"

#include "type_utils.h"

type_ptr<primitive_type> primitive_type::I16 = primitive_type::make(PRIMITIVE_I16);
type_ptr<primitive_type> primitive_type::I32 = primitive_type::make(PRIMITIVE_I32);
type_ptr<primitive_type> primitive_type::I64 = primitive_type::make(PRIMITIVE_I64);
type_ptr<primitive_type> primitive_type::FLOAT = primitive_type::make(PRIMITIVE_FLOAT);
type_ptr<primitive_type> primitive_type::DOUBLE = primitive_type::make(PRIMITIVE_DOUBLE);
type_ptr<primitive_type> primitive_type::CHAR = primitive_type::make(PRIMITIVE_CHAR);
type_ptr<primitive_type> primitive_type::WIDECHAR = primitive_type::make(PRIMITIVE_WIDECHAR);
type_ptr<primitive_type> primitive_type::STRING = primitive_type::make(PRIMITIVE_STRING);
type_ptr<primitive_type> primitive_type::WIDESTRING = primitive_type::make(PRIMITIVE_WIDESTRING);
type_ptr<primitive_type> primitive_type::BYTE = primitive_type::make(PRIMITIVE_BYTE);
type_ptr<primitive_type> primitive_type::BOOL = primitive_type::make(PRIMITIVE_BOOL);
type_ptr<primitive_type> primitive_type::VOID = primitive_type::make(PRIMITIVE_VOID);

bool primitive_type::operator==(const data_type_ptr other) const {
	if (auto other_prim = other->as_primitive()) {
		return prim == other_prim->prim;
	}
	return false;
}

int primitive_type::size_of() const {
	return size_of_primitive(prim);
}
std::string primitive_type::name() const {
	return to_string(prim);
}
