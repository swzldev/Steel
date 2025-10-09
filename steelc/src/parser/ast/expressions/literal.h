#pragma once

#include <string>
#include <memory>

#include "../ast_fwd.h"

class literal : public expression, public std::enable_shared_from_this<literal> {
public:
	ENABLE_ACCEPT(literal)

	literal(primitive_type primitive, std::string value)
		: primitive(primitive), value(value) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + value;
	}

	// accept is removed as this is a base class only,
	// in the future i may consider making this the only literal class
	// as all derived classes only use string values anyway

	virtual type_ptr type() const override {
		return std::make_shared<data_type>(data_type(primitive));
	}
	virtual bool is_rvalue() const override {
		return true; // literals are always constants
	}

	primitive_type primitive;
	std::string value;
};