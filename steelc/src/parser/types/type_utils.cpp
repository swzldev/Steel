#include "type_utils.h"

#include "../ast/declarations/type_declaration.h"

std::vector<std::shared_ptr<function_declaration>> get_method_candidates(std::shared_ptr<const type_declaration> type, const std::string& name, size_t arity) {
	std::vector<std::shared_ptr<function_declaration>> candidates;
	while (type != nullptr) {
		for (const auto& method : type->methods) {
			if (method->identifier == name && method->parameters.size() == arity) {
				candidates.push_back(method);
			}
		}
		type = type->base_class;
	}
	return candidates;
}