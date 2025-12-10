#pragma once

#include <string>
#include <vector>

#include "declaration.h"
#include "variable_declaration.h"
#include "../../parser_utils.h"
#include "../../types/data_type.h"

class conversion_declaration : public declaration, public std::enable_shared_from_this<conversion_declaration> {
public:
	ENABLE_ACCEPT(conversion_declaration)

	conversion_declaration(type_ptr from, type_ptr to, ast_ptr body)
		: from(from), to(to), body(body), implicit(false) {
	}
	conversion_declaration(type_ptr from, type_ptr to, ast_ptr body, bool implicit)
		: from(from), to(to), body(body), implicit(implicit) {
	}
	conversion_declaration(type_ptr from, type_ptr to, bool implicit)
		: from(from), to(to), body(nullptr), implicit(implicit) {
	}
	conversion_declaration(data_type_kind from, data_type_kind to, bool implicit = false)
		: from(to_data_type(from)), to(to_data_type(to)), body(nullptr), implicit(implicit) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Conversion Declaration:\n";
		result += ind + " From Type: " + from->name() + "\n";
		result += ind + " To Type: " + to->name() + "\n";
		result += ind + " Implicit: " + std::string(implicit ? "true" : "false") + "\n";
		if (body) {
			result += ind + " Body:\n";
			result += body->string(indent + 1);
		}
		else {
			result += ind + " Body: Not defined\n";
		}
		return result;
	}

	type_ptr from;
	type_ptr to;
	ast_ptr body;
	bool implicit;
};