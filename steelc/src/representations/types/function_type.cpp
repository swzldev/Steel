#include "function_type.h"

#include <string>

#include <representations/types/types_fwd.h>

std::shared_ptr<function_type> function_type::make(type_ptr return_type, std::vector<type_ptr> param_types) {
	return std::make_shared<function_type>(return_type, param_types);
}

bool function_type::operator==(const type_ptr& other) const
{
    if (!other->is_function()) {
        return false;
	}
	auto other_func = other->as_function();
	if (*return_type != other_func->return_type) {
		return false;
	}
	if (!params_match(other_func->parameter_types)) {
		return false;
	}
    return true;
}

int function_type::size_of() const {
	return -1; // n/a
	// only applicable to function pointers
}

std::string function_type::name() const {
	// format: (param1, param2, ...) -> return_type
	std::string result = "(";
	for (size_t i = 0; i < parameter_types.size(); i++) {
		result += parameter_types[i]->name();
		if (i < parameter_types.size() - 1) {
			result += ", ";
		}
	}
	result += ") -> " + return_type->name();
	return result;
}

std::shared_ptr<data_type> function_type::clone() const {
	return std::make_shared<function_type>(return_type->clone(), parameter_types);
}

bool function_type::params_match(const std::vector<type_ptr>& other_params) const {
	if (parameter_types.size() != other_params.size()) {
		return false;
	}
	for (size_t i = 0; i < parameter_types.size(); i++) {
		if (*parameter_types[i] != other_params[i]) {
			return false;
		}
	}
	return true;
}
