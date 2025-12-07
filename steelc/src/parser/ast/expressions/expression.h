#pragma once

#include <string>

#include "../ast_node.h"
#include "../../types/types_fwd.h"
#include "../../entities/entities_fwd.h"

class expression : public ast_node {
public:
	virtual ~expression() = default;

	std::string string(int indent) const override {
		return indent_s(indent) + "<Unknown Expression>";
	}

	// the type of this expression (can be null if this expression has no type)
	virtual type_ptr type() const = 0;
	// whether the expression is an rvalue (has no memory address)
	virtual bool is_rvalue() const = 0;
	// whether the expression is a constant value (evaluated at compile time)
	virtual bool is_constant() const = 0;

	// the entity this expression refers to (variable, function, type, etc)
	virtual entity_ptr entity() const { return nullptr; }
};