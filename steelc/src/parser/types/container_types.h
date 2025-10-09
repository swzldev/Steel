#pragma once

#include <memory>

#include "data_type.h"
#include "../ast/ast_fwd.h"

class array_type : public data_type {
public:
	array_type(data_type_ptr base_type)
		: base_type(base_type), data_type(DT_ARRAY) {
	}
	array_type(data_type_ptr base_type, size_t size)
		: base_type(base_type), data_type(DT_ARRAY) {
	}

	bool operator==(const data_type_ptr other) const override;

	std::string name() const override {
		return base_type->name() + "[]";
	}

	bool is_indexable() const override {
		return true;
	}

	data_type_ptr base_type;
	long long size = -1;
};

class pointer_type : public data_type {
public:
	pointer_type(data_type_ptr base_type)
		: base_type(base_type), data_type(DT_POINTER) {
	}

	bool operator==(const data_type_ptr other) const override;

	std::string name() const override {
		return base_type->name() + "*";
	}

	bool is_indexable() const override {
		return true;
	}

	data_type_ptr base_type;
};

class generic_type : public data_type {
public:
	generic_type(const std::string& identifier)
		: identifier(identifier), data_type(DT_GENERIC) {
	}

	bool operator==(const data_type_ptr other) const override;
	
	int size_of() const override {
		return -1;
	}
	std::string name() const override;

	bool is_indexable() const override {
		return false;
	}

	std::string identifier;
	std::shared_ptr<generic_parameter> declaration;
};