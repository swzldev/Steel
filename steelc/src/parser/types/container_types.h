#pragma once

#include <memory>

#include "types.h"

class array_type : public data_type {
public:
	array_type(type_ptr base_type)
		: base_type(base_type), data_type(DT_ARRAY) {
	}
	array_type(type_ptr base_type, size_t size)
		: base_type(base_type), data_type(DT_ARRAY) {
	}

	bool operator==(const data_type& other) const override;

	type_ptr base_type;
};

class pointer_type : public data_type {
public:
	pointer_type(type_ptr base_type)
		: base_type(base_type), data_type(DT_POINTER) {
	}

	bool operator==(const data_type& other) const override;

	type_ptr base_type;
};