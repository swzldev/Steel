#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../../parser_utils.h"

class deref_expression : public expression, public std::enable_shared_from_this<deref_expression> {
public:
	ENABLE_ACCEPT(deref_expression)

		deref_expression(std::shared_ptr<expression> value)
		: value(value) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Dereference Expression:\n";
		result += ind + " Value:\n" + value->string(indent + 1) + "\n";
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<deref_expression>(
			std::dynamic_pointer_cast<expression>(value->clone())
		);
		return cloned;
	}

	type_ptr type() const override {
		return make_pointer(value->type());
	}
	bool is_rvalue() const override {
		return true;
	}

	std::shared_ptr<expression> value;
};