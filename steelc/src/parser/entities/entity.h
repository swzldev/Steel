#pragma once

#include <memory>
#include <string>

#include "entities_fwd.h"

enum entity_kind {
	ENTITY_VARIABLE,
	ENTITY_FUNCTION,
	ENTITY_TYPE,
	ENTITY_MODULE,
};

class entity {
public:
	entity(entity_kind kind)
		: e_kind(kind) {
	}
	virtual ~entity() = default;

	inline entity_kind kind() const { return e_kind; }

	virtual std::string name() const = 0;
	virtual std::string full_name() const = 0;

	virtual std::shared_ptr<variable_entity> as_variable() { return nullptr; }
	virtual std::shared_ptr<function_entity> as_function() { return nullptr; }
	virtual std::shared_ptr<type_entity> as_type() { return nullptr; }
	virtual std::shared_ptr<module_entity> as_module() { return nullptr; }

private:
	entity_kind e_kind;
};