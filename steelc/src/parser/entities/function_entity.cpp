#include "function_entity.h"

#include "../ast/declarations/function_declaration.h"

std::shared_ptr<function_entity> function_entity::make(std::shared_ptr<function_declaration> declaration) {
	return std::shared_ptr<function_entity>(new function_entity(declaration));
}

std::string function_entity::name() const {
	return declaration->identifier;
}
std::string function_entity::full_name() const {
	return declaration->identifier;
}

std::shared_ptr<function_entity> function_entity::as_function() {
	return shared_from_this();
}
