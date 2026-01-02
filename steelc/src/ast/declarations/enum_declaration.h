#pragma once

#include <string>
#include <memory>
#include <vector>

#include <ast/ast_fwd.h>
#include <ast/declarations/declaration.h>
#include <ast/declarations/variable_declaration.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/operator_declaration.h>
#include <representations/types/enum_type.h>

class enum_declaration : public declaration, public std::enable_shared_from_this<enum_declaration> {
public:
	ENABLE_ACCEPT(enum_declaration)

	enum_declaration(const std::string& identifier, std::vector<std::shared_ptr<enum_option>> options)
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
			
		}
		return result;
	}

	type_ptr type() {
		if (!cached_type) {
			cached_type = std::make_shared<enum_type>(identifier, shared_from_this());
		}
		return cached_type;
	}
	std::string name() const {
		return identifier;
	}

	ast_ptr clone() const override {
		auto cloned = std::make_shared<enum_declaration>(identifier, options);
		cloned->span = span;
		cloned->base_type = base_type;
		return cloned;
	}

	std::string identifier;
	type_ptr base_type;
	std::vector<std::shared_ptr<enum_option>> options;

private:
	type_ptr cached_type;
};
