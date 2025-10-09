#pragma once

#include <string>
#include <memory>

#include "../../ast_node.h"
#include "../../expressions/expression.h"

class while_loop : public ast_node, public std::enable_shared_from_this<while_loop> {
public:
	ENABLE_ACCEPT(while_loop)

	while_loop(ast_ptr<expression> condition, ast_ptr<code_block> body)
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

	std::shared_ptr<expression> condition;
	ast_ptr<code_block> body;
};