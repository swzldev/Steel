#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>

class inline_if : public ast_node, public std::enable_shared_from_this<inline_if> {
public:
	ENABLE_ACCEPT(inline_if)

	inline_if(std::shared_ptr<expression> condition, std::shared_ptr<expression> statement)
		: condition(condition), statement(statement) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "If Statement:\n";
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		result += ind + " Statement:\n";
		if (statement) {
			result += statement->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<inline_if>(
			std::dynamic_pointer_cast<expression>(condition->clone()),
			std::dynamic_pointer_cast<expression>(statement->clone())
		);
		return cloned;
	}

	std::shared_ptr<expression> condition;
	std::shared_ptr<expression> statement;
};