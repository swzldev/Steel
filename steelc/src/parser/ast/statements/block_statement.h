#pragma once

#include <string>

#include "../ast_node.h"

class block_statement : public ast_node, public std::enable_shared_from_this<block_statement> {
public:
	ENABLE_ACCEPT(block_statement)

	block_statement() {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Block Statement:\n";
		result += ind + " Body:\n";
		if (body.size() <= 0) {
			result += ind + "  <Empty>\n";
			return result;
		}
		for (int i = 0; i < body.size(); i++) {
			auto& stmt = body[i];
			result += stmt->string(indent + 1) + "\n";
		}
		return result;
	}

	std::vector<ast_ptr> body;
};