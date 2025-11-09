#pragma once

#include "../ast_node.h"
#include "../ast_fwd.h"
#include "../declarations/enum_declaration.h"

class enum_option : public ast_node, public std::enable_shared_from_this<enum_option> {
public:
	ENABLE_ACCEPT(enum_option)

	enum_option(const std::string& identifier)
		: identifier(identifier) {
	}

	std::string string(int indent) const override {
		return indent_s(indent) + "Enum Option: " + identifier + "\n";
	}

	type_ptr type() {
		if (declaration) {
			return declaration->type();
		}
		return data_type::UNKNOWN;
	}
	std::string name() const {
		return identifier;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<enum_option>(identifier);
		cloned->declaration = declaration;
		return cloned;
	}

	std::string identifier;
	std::shared_ptr<enum_declaration> declaration;
};