#pragma once

#include <string>
#include <memory>
#include <vector>

#include "../parser_utils.h"

#define TEMP_PTR_SIZE 8

enum primitive_size {
	PS_SMALL,
	PS_NORMAL,
	PS_LARGE,
};

enum primitive_type {
	DT_UNKNOWN,

	DT_I16,
	DT_I32,
	DT_I64,
	DT_FLOAT,
	DT_DOUBLE,
	DT_CHAR,
	DT_WIDECHAR,
	DT_STRING,
	DT_WIDESTRING,
	DT_BYTE,
	DT_BOOL,
	DT_VOID,

	DT_ARRAY,
	DT_POINTER,
	DT_REFERENCE,
};

typedef unsigned int data_type_modifiers;
enum data_type_modifier {
	DTM_NONE = 0,
	DTM_CONST = 1 << 0,
};

class data_type {
public:
	data_type()
		: primitive(DT_UNKNOWN), modifiers(DTM_NONE) {
	}
	data_type(primitive_type primitive)
		: primitive(primitive), modifiers(DTM_NONE) {
	}

	static std::shared_ptr<data_type> unknown;

	virtual bool operator==(const data_type& other) const;
	virtual bool operator!=(const data_type& other) const;

	inline bool is_const() const {
		return modifiers & DTM_CONST;
	}
	inline bool is_array() const {
		return primitive == DT_ARRAY;
	}
	inline bool is_pointer() const {
		return primitive == DT_POINTER;
	}
	inline bool is_reference() const {
		return primitive == DT_REFERENCE;
	}
	virtual bool is_primitive() const;
	virtual bool is_indexable() const;
	virtual int size_of() const;
	virtual std::string type_name() const;

	primitive_type primitive; 
	data_type_modifiers modifiers;
};

using type_ptr = std::shared_ptr<data_type>;