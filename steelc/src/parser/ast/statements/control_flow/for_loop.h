#pragma once

#include <string>
#include <memory>

#include "../../ast_node.h"
#include "../../ast_fwd.h"

class for_loop : public ast_node, public std::enable_shared_from_this<for_loop> {
public:
	ENABLE_ACCEPT(for_loop)

	for_loop(ast_node_ptr initializer, ast_ptr<expression> condition, ast_ptr<expression> increment, ast_ptr<code_block> body)
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

	ast_node_ptr initializer;
	ast_ptr<expression> condition;
	ast_ptr<expression> increment;
	ast_ptr<code_block> body;
};