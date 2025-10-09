#pragma once

#include <string>
#include <vector>

#include "../ast_node.h"

class code_block : public ast_node, public std::enable_shared_from_this<code_block> {
public:
	ENABLE_ACCEPT(code_block)

	code_block(bool is_body)
		: is_body(is_body) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Block Statement:\n";
		result += ind + " Body:\n";
		if (statements.size() <= 0) {
			result += ind + "  <Empty>\n";
			return result;
		}
		for (int i = 0; i < statements.size(); i++) {
			auto& stmt = statements[i];
			result += stmt->string(indent + 1) + "\n";
		}
		return result;
	}

	bool is_body = false;
	std::vector<ast_node_ptr> statements;
};