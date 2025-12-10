#pragma once

#include <map>

#include "data_type.h"

class function_type : public data_type {
public:
	function_type(type_ptr return_type, std::vector<type_ptr> param_types)
		: data_type(DT_FUNCTION), return_type(return_type), parameter_types(param_types) {
	}

	static std::shared_ptr<function_type> make(type_ptr return_type, std::vector<type_ptr> param_types);

	bool operator==(const type_ptr& other) const override;

	int size_of() const override;

	bool is_primitive() const override {
		return false;
	}
	bool is_indexable() const override {
		return false;
	}

	std::string name() const override;

	std::shared_ptr<data_type> clone() const override;

	bool params_match(const std::vector<type_ptr>& other_params) const;

	inline const type_ptr& get_return_type() const {
		return return_type;
	}
	inline const std::vector<type_ptr>& get_parameter_types() const {
		return parameter_types;
	}

private:
	type_ptr return_type;
	std::vector<type_ptr> parameter_types;
};