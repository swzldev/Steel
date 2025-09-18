#pragma once

#include <string>
#include <memory>

#include "expression.h"

class cast_expression : public expression, public std::enable_shared_from_this<cast_expression> {
public:
	ENABLE_ACCEPT(cast_expression)

	cast_expression(type_ptr cast_type, std::shared_ptr<expression> expression)
		: cast_type(cast_type), expression(expression) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Cast Expression:\n";
		result += ind + " Cast Type: " + cast_type->name() + "\n";
		result += ind + " Expression: " + expression->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		if (!cast_type) {
			return data_type::unknown;
		}
		return cast_type;
	}
	bool is_rvalue() const override {
		return false;
	}

	type_ptr cast_type;
	std::shared_ptr<expression> expression;
};