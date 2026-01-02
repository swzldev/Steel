#pragma once

#include <string>
#include <memory>

#include <ast/expressions/expression.h>
#include <representations/types/type_utils.h>

class address_of_expression : public expression, public std::enable_shared_from_this<address_of_expression> {
public:
	ENABLE_ACCEPT(address_of_expression)

	address_of_expression(std::shared_ptr<expression> value)
		: value(value) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Address Of Expression:\n";
		result += ind + " Value:\n" + value->string(indent + 1) + "\n";
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<address_of_expression>(
			std::dynamic_pointer_cast<expression>(value->clone())
		);
		cloned->span = span;
		return cloned;
	}

	type_ptr type() const override {
		return make_pointer(value->type());
	}
	bool is_rvalue() const override {
		return true;
	}
	bool is_constant() const override {
		return false;
	}

	std::shared_ptr<expression> value;
};