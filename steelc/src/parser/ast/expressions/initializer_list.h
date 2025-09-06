#pragma once

#include <string>
#include <memory>

#include "expression.h"
#include "../../parser_utils.h"
#include "../../types/types.h"

class initializer_list : public expression, public std::enable_shared_from_this<initializer_list> {
public:
	ENABLE_ACCEPT(initializer_list)

	initializer_list(std::vector<std::shared_ptr<expression>> args)
		: values(args) {
	}

	std::string string(int indent) const override {
		std::string ind = indent_s(indent);
		std::string result = ind + "Initializer List:\n";
		result += ind + " Values:\n";
		if (values.size() <= 0) {
			result += ind + "  <None>\n";
		}
		else {
			for (int i = 0; i < values.size(); i++) {
				result += values[i]->string(indent + 1) + "\n";
			}
		}
		return result;
	}

	type_ptr type() const override {
		return result_type ? result_type : data_type::unknown;
	}
	bool is_rvalue() const override {
		return true; // initializer list always return a temporary value
		// really, we are never going to check this as this can only be used
		// on the right hand of an assignment
	}

	std::vector<std::shared_ptr<expression>> values;
	type_ptr result_type;
};