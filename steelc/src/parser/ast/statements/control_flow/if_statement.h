#pragma once

#include <string>
#include <memory>

#include "../../ast_node.h"
#include "../../expressions/expression.h"

class if_statement : public ast_node, public std::enable_shared_from_this<if_statement> {
public:
	ENABLE_ACCEPT(if_statement)

	if_statement()
		: condition(nullptr), then_block(nullptr), else_node(nullptr) {
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
		result += ind + " Then Block:\n";
		if (then_block) {
			result += then_block->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		result += ind + " Else Block:\n";
		if (else_node) {
			result += else_node->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<if_statement>();
		if (condition) {
			cloned->condition = std::dynamic_pointer_cast<expression>(condition->clone());
		}
		if (then_block) {
			cloned->then_block = then_block->clone();
		}
		if (else_node) {
			cloned->else_node = else_node->clone();
		}
		return cloned;
	}

	std::shared_ptr<expression> condition;
	ast_ptr then_block;
	ast_ptr else_node;
};