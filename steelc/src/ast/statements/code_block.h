#pragma once

#include <string>
#include <vector>

#include <ast/ast_node.h>

class code_block : public ast_node, public std::enable_shared_from_this<code_block> {
public:
	ENABLE_ACCEPT(code_block)

	code_block() {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Code block:\n";
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

	ast_ptr clone() const override {
		auto cloned = std::make_shared<code_block>();
		cloned->span = span;
		cloned->is_body = is_body;
		for (const auto& stmt : body) {
			cloned->body.push_back(stmt->clone());
		}
		return cloned;
	}

	std::vector<ast_ptr> body;
	bool is_body = false;
};