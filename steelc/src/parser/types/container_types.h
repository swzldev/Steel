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

	std::string name() const override {
		return base_type->name() + "[]";
	}

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return true;
	}

	type_ptr base_type;
};

class pointer_type : public data_type {
public:
	pointer_type(type_ptr base_type)
		: base_type(base_type), data_type(DT_POINTER) {
	}

	bool operator==(const data_type& other) const override;

	std::string name() const override {
		return base_type->name() + "*";
	}

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return true;
	}

	type_ptr base_type;
};

class generic_type : public data_type {
public:
	generic_type(const std::string& identifier)
		: identifier(identifier), data_type(DT_GENERIC) {
	}

	std::string identifier;

	bool operator==(const data_type& other) const override;
	
	std::string name() const override {
		if (substitution) {
			return substitution->name();
		}
		return identifier;
	}

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return false;
	}

	type_ptr substitution = nullptr;
};