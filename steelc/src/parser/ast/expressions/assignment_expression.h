#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../../types/data_type.h"
#include "../../../lexer/token_type.h"
#include "../../../lexer/token_utils.h"

class assignment_expression : public expression, public std::enable_shared_from_this<assignment_expression> {
public:
	ENABLE_ACCEPT(assignment_expression)

	assignment_expression(std::shared_ptr<expression> left, std::shared_ptr<expression> right)
		: left(left), right(right) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Assignment Expression:\n";
		result += ind + " Assignee:\n" + left->string(indent + 1) + "\n";
		result += ind + " Value:\n" + right->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		return left->type();
	}
	bool is_rvalue() const override {
		return true;
	}

	std::shared_ptr<expression> left;
	std::shared_ptr<expression> right;
};