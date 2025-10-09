#pragma once

#include <string>
#include <memory>

#include "../../ast_node.h"
#include "../../ast_fwd.h"

class if_statement : public ast_node, public std::enable_shared_from_this<if_statement> {
public:
	ENABLE_ACCEPT(if_statement)

	if_statement()
		: condition(nullptr), then_block(nullptr), else_statement(nullptr) {
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
		if (else_statement) {
			result += else_statement->string(indent + 1) + "\n";
		}
		else {
			result += ind + "  <Empty>\n";
		}
		return result;
	}

	ast_ptr<expression> condition;
	ast_ptr<code_block> then_block;
	ast_node_ptr else_statement;
};