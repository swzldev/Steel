#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../../types/data_type.h"
#include "../../types/container_types.h"
#include "../../parser_utils.h"
#include "../../../lexer/token_type.h"
#include "../../../lexer/token_utils.h"

class index_expression : public expression, public std::enable_shared_from_this<index_expression> {
public:
	ENABLE_ACCEPT(index_expression)
		
	index_expression(std::shared_ptr<expression> base, std::shared_ptr<expression> indexer)
		: base(base), indexer(indexer) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Index Expression:\n";
		result += ind + " Base:\n" + base->string(indent + 1) + "\n";
		result += ind + " Indexer:\n" + indexer->string(indent + 1) + "\n";
		return result;
	}

	type_ptr type() const override {
		auto t = base->type();
		if (t->is_array()) {
			return std::dynamic_pointer_cast<array_type>(t)->base_type;
		}
		else if (t->is_pointer()) {
			return std::dynamic_pointer_cast<pointer_type>(t)->base_type;
		}
		else if (t->is_primitive() && t->primitive == DT_STRING) {
			return to_data_type(DT_CHAR);
		}
		return base->type();
	}
	bool is_rvalue() const override {
		return true;
	}


	std::shared_ptr<expression> base;
	std::shared_ptr<expression> indexer;
};