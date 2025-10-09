#pragma once

#include <vector>
#include <string>

#include "../ast_node.h"
#include "../ast_fwd.h"
#include "../../types/type_utils.h"
#include "../../../lexer/token_utils.h"

class type_node : public ast_node, public std::enable_shared_from_this<type_node> {
public:
	ENABLE_ACCEPT(type_node)

	enum class kind {
		BASE,
		ARRAY,
		POINTER,
		REFERENCE,
	};

	type_node()
		: type_name("unknown"), kind(kind::BASE) {
	}
	type_node(const std::string& type_name)
	: type_name(type_name), kind(kind::BASE) {
	}
	type_node(ast_ptr<type_node> base_type, kind kind)
		: base_type(base_type), kind(kind) {
	}
	type_node(ast_ptr<type_node> base_type, ast_ptr<expression> array_size)
		: base_type(base_type), kind(kind::ARRAY), array_size(array_size) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Type Node:\n";
		return result;
	}

	ast_ptr<type_node> base_type;
	std::string type_name;
	std::vector<data_type_modifier> modifiers;
	kind kind;
	ast_ptr<expression> array_size; // only for arrays, null for dynamic arrays
};