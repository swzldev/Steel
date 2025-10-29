#pragma once

#include <memory>
#include <string>

#include "ast_visitor.h"
#include "../../lexer/token.h"
#include "../../utils/iclonable.h"

#define ENABLE_ACCEPT(T) \
	void accept(ast_visitor& visitor) override { \
		visitor.visit(shared_from_this()); \
	}

class ast_node;
using ast_ptr = std::shared_ptr<ast_node>;

class ast_node : public iclonable<ast_node> {
public:
	virtual ~ast_node() = default;

	virtual std::string string(int indent = 0) const = 0;

	virtual void accept(ast_visitor& visitor) = 0;

	// this doesnt need to be overridden at least for now so i dont
	// have to make a clone method for every ast node
	virtual ast_ptr clone() const override {
		return nullptr;
	}

	position position = { 0, 0 };

protected:
	 std::string indent_s(int level) const {
		return std::string(level * 4, ' ');
	}
};
