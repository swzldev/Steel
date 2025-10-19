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
		: identifier(identifier), type(type) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Generic Parameter: '" + identifier + "'";
	}

	void add_reference(const std::shared_ptr<class generic_type>& gen_type) {
		references.push_back(gen_type);
	}

	std::string identifier;
	generic_param_type type;
	type_ptr substitution;
	std::vector<std::shared_ptr<class generic_type>> references;
};