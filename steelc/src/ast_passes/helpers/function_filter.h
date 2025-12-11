#pragma once

#include <memory>
#include <vector>

#include <representations/entities/entities_fwd.h>

class function_filter {
public:
	function_filter(const std::vector<std::shared_ptr<function_entity>>& functions)
		: functions(functions) {
	}

	std::vector<std::shared_ptr<function_entity>> filter_by_param_count(size_t param_count);
	std::vector<std::shared_ptr<function_entity>> filter_by_param_count(size_t param_count, size_t generic_count);
	
private:
	std::vector<std::shared_ptr<function_entity>> functions;
};