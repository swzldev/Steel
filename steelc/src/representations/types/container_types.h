#pragma once

#include <memory>
#include <string>

#include <representations/types/data_type.h>
#include <ast/expressions/expression.h>

class array_type : public data_type {
public:
	array_type(type_ptr base_type)
		: base_type(base_type), data_type(DT_ARRAY), size_expression(nullptr) {
	}
	array_type(type_ptr base_type, std::shared_ptr<expression> size_expr)
		: base_type(base_type), data_type(DT_ARRAY), size_expression(size_expr) {
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
	std::shared_ptr<expression> size_expression;
	size_t computed_size = 0;
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

	int generic_param_index = 0;
};