#pragma once

#include <string>
#include <memory>
#include <vector>

#include "../parser_utils.h"

class data_type;
class custom_type;
class primitive_type;
class array_type;
class pointer_type;
class generic_type;

using data_type_ptr = std::shared_ptr<data_type>;
template<typename T>
using type_ptr = std::shared_ptr<T>;

typedef unsigned int data_type_modifiers;
enum data_type_modifier {
	DTM_NONE = 0,
};

enum data_type_kind {
	DT_UNKNOWN,
	DT_PRIMITIVE,
	DT_CUSTOM,
	DT_ARRAY,
	DT_POINTER,
	DT_REFERENCE,
	DT_GENERIC,
};

class data_type : public std::enable_shared_from_this<data_type> {
public:
	static std::shared_ptr<data_type> UNKNOWN;

	virtual bool operator==(const data_type_ptr other) const = 0;
	virtual bool operator!=(const data_type_ptr other) const;

	virtual int size_of() const = 0;
	virtual std::string name() const = 0;

	inline bool is_primitive() const {
		return kind == DT_PRIMITIVE;
	}
	inline bool is_custom() const {
		return kind == DT_CUSTOM;
	}
	inline bool is_array() const {
		return kind == DT_ARRAY;
	}
	inline bool is_pointer() const {
		return kind == DT_POINTER;
	}
	inline bool is_reference() const {
		return kind == DT_REFERENCE;
	}
	inline bool is_generic() const {
		return kind == DT_GENERIC;
	}

	inline type_ptr<primitive_type> as_primitive() const;
	inline type_ptr<custom_type> as_custom() const;
	inline type_ptr<array_type> as_array() const;
	inline type_ptr<pointer_type> as_pointer() const;
	inline void* as_reference() const {
		return nullptr;
	}
	inline type_ptr<generic_type> as_generic() const;

	virtual bool is_indexable() const = 0;

	data_type_kind kind;
	data_type_modifiers modifiers;

protected:
	data_type()
		: kind(DT_UNKNOWN), modifiers() {
	}
	data_type(data_type_kind kind)
		: kind(kind), modifiers() {
	}
};
