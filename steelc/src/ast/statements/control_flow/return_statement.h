#pragma once

#include <string>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>

class return_statement : public ast_node, public std::enable_shared_from_this<return_statement> {
public:
	ENABLE_ACCEPT(return_statement)

	return_statement()
		: value(nullptr), condition(nullptr) {
	}
	return_statement(std::shared_ptr<expression> value)
		: value(value), condition(nullptr) {
	}
	return_statement(std::shared_ptr<expression> value, std::shared_ptr<expression> condition)
		: value(value), condition(condition) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Return Statement:\n";
		result += ind + " Return Value:\n";
		if (returns_value()) {
			result += value->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<return_statement>();
		if (value) {
			cloned->value = std::dynamic_pointer_cast<expression>(value->clone());
		}
		if (condition) {
			cloned->condition = std::dynamic_pointer_cast<expression>(condition->clone());
		}
		return cloned;
	}

	inline bool is_conditional() const {
		return condition != nullptr;
	}
	inline bool returns_value() const {
		return value != nullptr;
	}

	std::shared_ptr<expression> value;
	std::shared_ptr<expression> condition;
};