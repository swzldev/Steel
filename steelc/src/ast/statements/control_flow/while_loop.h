#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>

class while_loop : public ast_node, public std::enable_shared_from_this<while_loop> {
public:
	ENABLE_ACCEPT(while_loop)

	while_loop(std::shared_ptr<expression> condition, ast_ptr body)
		: condition(condition), body(body) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "While loop:\n";
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		result += ind + " Body:\n";
		if (body) {
			result += body->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<while_loop>(
			std::dynamic_pointer_cast<expression>(condition->clone()),
			body->clone()
		);
		cloned->position = position;
		return cloned;
	}

	std::shared_ptr<expression> condition;
	ast_ptr body;
};