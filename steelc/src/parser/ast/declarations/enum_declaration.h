#pragma once

#include <string>
#include <memory>
#include <vector>

#include "declaration.h"
#include "variable_declaration.h"
#include "function_declaration.h"
#include "operator_declaration.h"
#include "../../types/enum_type.h"

struct enum_option {
	std::string identifier;
};

class enum_declaration : public declaration, public std::enable_shared_from_this<enum_declaration> {
public:
	ENABLE_ACCEPT(enum_declaration)

	enum_declaration(const std::string& identifier, std::vector<enum_option> options)
		: identifier(identifier), options(options) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Enum Declaration:\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Options:\n";
		if (options.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& option : options) {
				result += ind + "  - " + option.identifier + "\n";
			}
		}
		return result;
	}

	type_ptr type() {
		static type_ptr type;
		if (!type) {
			type = std::make_shared<enum_type>(identifier, shared_from_this());
		}
		return type;
	}
	std::string name() const {
		return identifier;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<enum_declaration>(identifier, options);
		cloned->base_type = base_type;
		return cloned;
	}

	std::string identifier;
	type_ptr base_type;
	std::vector<enum_option> options;
};