#pragma once

#include <string>
#include <memory>

#include <ast/expressions/expression.h>

class cast_expression : public expression, public std::enable_shared_from_this<cast_expression> {
public:
	ENABLE_ACCEPT(cast_expression)

	cast_expression(type_ptr cast_type, std::shared_ptr<expression> expr)
		: cast_type(cast_type), expr(expr) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Cast Expression:\n";
		result += ind + " Cast Type: " + cast_type->name() + "\n";
		result += ind + " Expression: " + expr->string(indent + 1) + "\n";
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<cast_expression>(
			cast_type,
			std::dynamic_pointer_cast<expression>(expr->clone())
		);
		return cloned;
	}

	type_ptr type() const override {
		if (!cast_type) {
			return data_type::UNKNOWN;
		}
		return cast_type;
	}
	bool is_rvalue() const override {
		return false;
	}
	bool is_constant() const override {
		return expr->is_constant();
	}

	type_ptr cast_type;
	std::shared_ptr<expression> expr;
};