#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../../types/types.h"
#include "../../../lexer/token_type.h"
#include "../../../lexer/token_utils.h"

class unary_expression : public expression, public std::enable_shared_from_this<unary_expression> {
public:
	ENABLE_ACCEPT(unary_expression)

	unary_expression(token_type op, std::shared_ptr<expression> operand)
		: oparator(op), operand(operand) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Binary Expression:\n";
		result += ind + " Operand:\n" + operand->string(indent + 1) + "\n";
		result += ind + " Operator: " + to_string(oparator) + "\n";
		return result;
	}

	type_ptr type() const override {
		return operand->type();
	}
	bool is_rvalue() const override {
		return false;
	}

	token_type oparator;
	std::shared_ptr<expression> operand;
};