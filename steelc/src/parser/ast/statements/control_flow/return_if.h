#pragma once

#include <string>

#include "../../ast_node.h"
#include "../../expressions/expression.h"

class return_if : public ast_node, public std::enable_shared_from_this<return_if> {
public:
	ENABLE_ACCEPT(return_if)

	return_if(std::shared_ptr<expression> condition)
		: condition(condition), value(nullptr) {
	}
	return_if(std::shared_ptr<expression> condition, std::shared_ptr<expression> value)
		: condition(condition), value(value) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Return Statement:\n";
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Return Value:\n";
		if (returns_value()) {
			result += value->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	inline bool returns_value() const {
		return value != nullptr;
	}

	std::shared_ptr<expression> condition;
	std::shared_ptr<expression> value;
};