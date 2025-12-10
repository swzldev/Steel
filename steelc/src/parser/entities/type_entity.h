#pragma once

#include <memory>
#include <string>

#include "entity.h"
#include "../types/types_fwd.h"
#include "../symbolics/type_symbols.h"

class type_entity : public entity, public std::enable_shared_from_this<type_entity> {
public:
	static std::shared_ptr<type_entity> make(type_ptr type);

	std::string name() const override;
	std::string full_name() const override;

	std::shared_ptr<type_entity> as_type() override;

	type_ptr type;
	type_symbols symbols;

private:
	type_entity(type_ptr type)
		: entity(ENTITY_TYPE), type(type) {
	}
};