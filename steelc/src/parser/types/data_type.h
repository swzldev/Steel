#pragma once

#include <string>
#include <memory>
#include <vector>

#include "../../lexer/token.h"
#include "../../utils/iclonable.h"

enum primitive_type {
	DT_UNKNOWN,
	DT_CUSTOM,

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

	// special type for generic type params
	DT_GENERIC,
};

enum data_type_modifier {
	DTM_NONE = 0,
};

// type forward declarations
class data_type;
class custom_type;
class array_type;
class pointer_type;
class generic_type;

using type_ptr = std::shared_ptr<data_type>;

class data_type : public iclonable<data_type>, public std::enable_shared_from_this<data_type> {
public:
	data_type()
		: primitive(DT_UNKNOWN), modifiers(DTM_NONE), position(0, 0) {
	}
	data_type(primitive_type primitive)
		: primitive(primitive), modifiers(DTM_NONE), position(0, 0) {
	}

	static std::shared_ptr<data_type> UNKNOWN;

	virtual bool operator==(const type_ptr& other) const;
	bool operator!=(const type_ptr& other) const;

	inline bool is_unknown() const {
		return primitive == DT_UNKNOWN;
	}
	inline bool is_void() const {
		return primitive == DT_VOID;
	}
	inline bool is_integer() const {
		return primitive == DT_I16 || primitive == DT_I32 || primitive == DT_I64;
	}
	inline bool is_floating_point() const {
		return primitive == DT_FLOAT || primitive == DT_DOUBLE;
	}
	inline bool is_numeric() const {
		return is_integer() || is_floating_point();
	}
	inline bool is_character() const {
		return primitive == DT_CHAR || primitive == DT_WIDECHAR;
	}
	inline bool is_text() const {
		return primitive == DT_STRING || primitive == DT_WIDESTRING;
	}
	inline bool is_custom() const {
		return primitive == DT_CUSTOM;
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
	inline bool is_generic() const {
		return primitive == DT_GENERIC;
	}

	std::shared_ptr<custom_type> as_custom();
	std::shared_ptr<array_type> as_array();
	std::shared_ptr<pointer_type> as_pointer();
	std::shared_ptr<data_type> as_reference();
	std::shared_ptr<generic_type> as_generic();

	virtual bool is_primitive() const;
	virtual bool is_indexable() const;

	virtual int size_of() const;

	virtual std::string name() const;

	virtual std::shared_ptr<data_type> clone() const override;

	primitive_type primitive; 
	std::vector<data_type_modifier> modifiers;
	position position;
};
