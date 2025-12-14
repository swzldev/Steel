#pragma once

#include <memory>
#include <string>
#include <vector>

#include <representations/entities/entity.h>
#include <representations/types/types_fwd.h>
#include <ast/ast_fwd.h>

class function_entity : public entity, public std::enable_shared_from_this<function_entity> {
public:
	static std::shared_ptr<function_entity> get(std::shared_ptr<function_declaration> declaration);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<function_entity> as_function() override;

	// shorthands
	std::shared_ptr<function_type> function_type() const;
	size_t param_count() const;
	size_t generic_param_count() const;
	std::vector<type_ptr> param_types() const;

	std::shared_ptr<function_declaration> declaration;

private:
	function_entity(std::shared_ptr<function_declaration> declaration)
		: entity(ENTITY_FUNCTION), declaration(declaration) {
	}
};