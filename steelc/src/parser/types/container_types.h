#pragma once

#include <memory>

#include "data_type.h"
#include "../ast/generics/generic_parameter.h"

class array_type : public data_type {
public:
	array_type(type_ptr base_type)
		: base_type(base_type), data_type(DT_ARRAY) {
	}
	array_type(type_ptr base_type, size_t size)
		: base_type(base_type), data_type(DT_ARRAY) {
	}

	bool operator==(const type_ptr& other) const override;

	std::string name() const override {
		return base_type->name() + "[]";
	}

	std::shared_ptr<data_type> clone() const override {
		return std::make_shared<array_type>(*this);
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

	bool operator==(const type_ptr& other) const override;

	std::string name() const override {
		return base_type->name() + "*";
	}

	std::shared_ptr<data_type> clone() const override {
		return std::make_shared<pointer_type>(*this);
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

	bool operator==(const type_ptr& other) const override;
	
	std::string name() const override {
		if (declaration && declaration->substitution) {
			return declaration->substitution->name();
		}
		return identifier;
	}

	std::shared_ptr<data_type> clone() const override {
		return std::make_shared<generic_type>(*this);
	}

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return false;
	}

	std::shared_ptr<generic_parameter> declaration = nullptr;
};