#pragma once

#include <string>
#include <memory>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>

class for_loop : public ast_node, public std::enable_shared_from_this<for_loop> {
public:
	ENABLE_ACCEPT(for_loop)

	for_loop(ast_ptr initializer, std::shared_ptr<expression> condition, std::shared_ptr<expression> increment, ast_ptr body)
		: initializer(initializer), condition(condition), increment(increment), body(body) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "For loop:\n";
		result += ind + " Initializer:\n";
		if (initializer) {
			result += initializer->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Condition:\n";
		if (condition) {
			result += condition->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Increment:\n";
		if (increment) {
			result += increment->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		result += ind + " Body:\n";
		if (body) {
			result += body->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <None>\n";
		}
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<for_loop>(
			initializer->clone(),
			std::dynamic_pointer_cast<expression>(condition->clone()),
			std::dynamic_pointer_cast<expression>(increment->clone()),
			body->clone()
		);
		cloned->position = position;
		return cloned;
	}

	ast_ptr initializer;
	std::shared_ptr<expression> condition;
	std::shared_ptr<expression> increment;
	ast_ptr body;
};