#pragma once

#include <memory>
#include <string>

#include <representations/entities/entity.h>
#include <ast/ast_fwd.h>

class generic_param_entity : public entity, public std::enable_shared_from_this<generic_param_entity> {
public:
	static std::shared_ptr<generic_param_entity> get(std::shared_ptr<generic_parameter> gp);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<generic_param_entity> as_generic_param() override;

	std::shared_ptr<generic_parameter> declaration;

private:
	generic_param_entity(std::shared_ptr<generic_parameter> gp)
		: entity(ENTITY_GENERIC_PARAM), declaration(gp) {
	}
};