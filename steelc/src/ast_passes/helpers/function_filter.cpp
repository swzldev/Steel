#include "function_filter.h"

#include <vector>
#include <memory>

#include <ast/declarations/function_declaration.h>
#include <representations/entities/function_entity.h>

std::vector<std::shared_ptr<function_entity>> function_filter::filter_by_param_count(size_t param_count) {
	std::vector<std::shared_ptr<function_entity>> result;
	for (const auto& func : functions) {
		if (func->param_count() == param_count) {
			result.push_back(func);
		}
	}
	return result;
}
std::vector<std::shared_ptr<function_entity>> function_filter::filter_by_param_count(size_t param_count, size_t generic_count) {
	std::vector<std::shared_ptr<function_entity>> result;
	auto param_filtered = filter_by_param_count(param_count);
	for (const auto& func : param_filtered) {
		if (func->declaration->generics.size() == generic_count) {
			result.push_back(func);
		}
	}
	return result;
}
