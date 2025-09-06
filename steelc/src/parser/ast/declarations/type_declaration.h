#pragma once

#include <string>
#include <memory>
#include <vector>

#include "declaration.h"
#include "constructor_declaration.h"
#include "variable_declaration.h"
#include "function_declaration.h"
#include "operator_declaration.h"
#include "../../types/custom_types.h"

class type_declaration : public declaration, public std::enable_shared_from_this<type_declaration> {
public:
	ENABLE_ACCEPT(type_declaration)

	type_declaration(custom_type_type kind, const std::string& identifier)
		: identifier(identifier), type_kind(kind) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Type Declaration:\n";
		result += ind + " Identifier: " + identifier + "\n";
		result += ind + " Type Kind: " + (type_kind == CT_STRUCT ? "Struct" : type_kind == CT_CLASS ? "Class" : type_kind == CT_INTERFACE ? "Interface" : "Unknown") + "\n";
		result += ind + " Constructors:\n";
		if (constructors.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& constructor : constructors) {
				result += constructor->string(indent + 1) + "\n";
			}
		}
		result += ind + " Members:\n";
		if (fields.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& member : fields) {
				result += member->string(indent + 1) + "\n";
			}
		}
		result += ind + " Methods:\n";
		if (methods.empty()) {
			result += ind + "  <None>\n";
		}
		else {
			for (const auto& method : methods) {
				result += method->string(indent + 1) + "\n";
			}
		}
		return result;
	}

	type_ptr type() const {
		auto t = cached_type.lock();
		if (!t) {
			t = std::make_shared<custom_type>(shared_from_this(), identifier);
			cached_type = t;
		}
		return t;
	}
	std::string type_name() const {
		return identifier;
	}

	bool is_indexable() const {
		return false; // TODO
	}

	std::string identifier;
	custom_type_type type_kind;
	std::vector<std::shared_ptr<constructor_declaration>> constructors;
	std::vector<std::shared_ptr<variable_declaration>> fields;
	std::vector<std::shared_ptr<function_declaration>> methods;
	std::vector<std::shared_ptr<operator_declaration>> operators;

private:
	mutable std::weak_ptr<custom_type> cached_type;
};