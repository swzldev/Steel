#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../../parser_utils.h"

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

	type_ptr type() const override {
		return make_pointer(value->type());
	}
	bool is_rvalue() const override {
		return true;
	}

	std::shared_ptr<expression> value;
};