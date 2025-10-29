#pragma once

#include <string>
#include <memory>

#include "../ast_node.h"
#include "../../types/data_type.h"

enum generic_param_type {
	GENERIC_TYPE,
	GENERIC_VALUE,
};

class generic_parameter : public ast_node, public std::enable_shared_from_this<generic_parameter> {
public:
	ENABLE_ACCEPT(generic_parameter)
	
	generic_parameter(const std::string& identifier, generic_param_type type = GENERIC_TYPE)
		: identifier(identifier), type(type), param_index(0), substitution(data_type::UNKNOWN) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Generic Parameter: '" + identifier + "'";
	}

	ast_ptr clone() const override { 
		auto cloned = std::make_shared<generic_parameter>(identifier, type);
		cloned->param_index = param_index;
		return cloned;
	}

	std::string identifier;
	generic_param_type type;
	int param_index;
	// (doesnt actually do anything, purely for type rememering)
	type_ptr substitution;
};