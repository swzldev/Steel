#pragma once

#include <memory>
#include <string>

#include "../../lexer/token.h"
#include "ast_visitor.h"

#define ENABLE_ACCEPT(T) \
	void accept(ast_visitor& visitor) override { \
		visitor.visit(shared_from_this()); \
	}

class ast_node {
public:
	virtual ~ast_node() = default;

	virtual std::string string(int indent = 0) const = 0;

	virtual void accept(ast_visitor& visitor) = 0;

	position position = { 0, 0 };

protected:
	 std::string indent_s(int level) const {
		return std::string(level * 4, ' ');
	}
};

using ast_ptr = std::shared_ptr<ast_node>;