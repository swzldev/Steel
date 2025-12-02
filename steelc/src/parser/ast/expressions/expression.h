#pragma once

#include <string>

#include "../ast_node.h"
#include "../../types/data_type.h"

class expression : public ast_node {
public:
	virtual ~expression() = default;

	std::string string(int indent) const override {
		return indent_s(indent) + "<Unknown Expression>";
	}

	virtual type_ptr type() const = 0;
	virtual bool is_rvalue() const = 0;
	virtual bool is_constant() const = 0;
};