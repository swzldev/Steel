#pragma once

#include <string>

#include <ast/ast_node.h>
#include <ast/expressions/expression.h>

class break_statement : public ast_node, public std::enable_shared_from_this<break_statement> {
public:
	ENABLE_ACCEPT(break_statement)

	break_statement()
		: condition(nullptr) {
	}
	break_statement(std::shared_ptr<expression> condition)
		: condition(condition) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Break Statement:\n";
		return result;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<break_statement>();
		if (condition) {
			cloned->condition = std::dynamic_pointer_cast<expression>(condition->clone());
		}
		return cloned;
	}

	inline bool is_conditional() const {
		return condition != nullptr;
	}

	std::shared_ptr<expression> condition;
};