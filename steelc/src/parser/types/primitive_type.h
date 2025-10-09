#pragma once

#include "data_type.h"

enum primitive {
	PRIMITIVE_I16,
	PRIMITIVE_I32,
	PRIMITIVE_I64,
	PRIMITIVE_FLOAT,
	PRIMITIVE_DOUBLE,
	PRIMITIVE_CHAR,
	PRIMITIVE_WIDECHAR,
	PRIMITIVE_STRING,
	PRIMITIVE_WIDESTRING,
	PRIMITIVE_BYTE,
	PRIMITIVE_BOOL,
	PRIMITIVE_VOID,
};

class primitive_type : public data_type {
public:
	static inline type_ptr<primitive_type> make(primitive primitive) {
		return std::make_shared<primitive_type>(primitive);
	}

	static type_ptr<primitive_type> I16;
	static type_ptr<primitive_type> I32;
	static type_ptr<primitive_type> I64;
	static type_ptr<primitive_type> FLOAT;
	static type_ptr<primitive_type> DOUBLE;
	static type_ptr<primitive_type> CHAR;
	static type_ptr<primitive_type> WIDECHAR;
	static type_ptr<primitive_type> STRING;
	static type_ptr<primitive_type> WIDESTRING;
	static type_ptr<primitive_type> BYTE;
	static type_ptr<primitive_type> BOOL;
	static type_ptr<primitive_type> VOID;

	bool operator==(const data_type_ptr other) const override;

	int size_of() const override;
	std::string name() const override;

	bool is_indexable() const override {
		return prim == PRIMITIVE_STRING || prim == PRIMITIVE_WIDESTRING;
	}

	primitive prim;

private:
	primitive_type(primitive prim)
		: prim(prim), data_type(DT_PRIMITIVE) {
	}
};