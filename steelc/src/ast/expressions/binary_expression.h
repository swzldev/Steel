#pragma once

#include <string>
#include <memory>

#include <ast/expressions/expression.h>
#include <lexer/token_type.h>
#include <lexer/token_utils.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>

class binary_expression : public expression, public std::enable_shared_from_this<binary_expression> {
public:
	ENABLE_ACCEPT(binary_expression)

	binary_expression(std::shared_ptr<expression> left, std::shared_ptr<expression> right, token_type oparater)
		: left(left), right(right), oparator(oparater) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Binary Expression:\n";
		result += ind + " Operator: " + to_string(oparator) + "\n";
		result += ind + " Left:\n" + left->string(indent + 1) + "\n";
		result += ind + " Right:\n" + right->string(indent + 1) + "\n";
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<binary_expression>(
			std::dynamic_pointer_cast<expression>(left->clone()),
			std::dynamic_pointer_cast<expression>(right->clone()),
			oparator
		);
		cloned->position = position;
		cloned->result_type = result_type;
		return cloned;
	}

	type_ptr type() const override {
		if (!result_type) {
			return data_type::UNKNOWN;
		}
		return result_type;
	}
	bool is_rvalue() const override {
		return true;
	}
	bool is_constant() const override {
		return left->is_constant() && right->is_constant();
	}

	std::shared_ptr<expression> left;
	std::shared_ptr<expression> right;
	token_type oparator;
    type_ptr result_type;
};