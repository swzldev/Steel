#pragma once

#include <memory>
#include <string>

#include <representations/entities/entities_fwd.h>

enum entity_kind {
	// reserved for unresolved entities
	// shouldnt really be used directly
	// just compare to entity::UNRESOLVED
	ENTITY_UNRESOLVED,

	ENTITY_VARIABLE,
	ENTITY_FUNCTION,
	ENTITY_TYPE,
	ENTITY_MODULE,
	ENTITY_GENERIC_PARAM,
};

class entity {
public:
	virtual ~entity() = default;

	// acts seperate from a nullptr entity for unresolved references
	// e.g. 3 + 3 -> nullptr entity (no entity) vs unknown_var -> UNRESOLVED entity
	static entity_ptr UNRESOLVED;

	inline entity_id get_id() const { return id; }
	inline entity_kind kind() const { return e_kind; }
	std::string kind_string() const;

	virtual std::string name() const { return "<unknown entity>"; }
	virtual std::string full_name() const { return "<unknown entity>"; }

	virtual std::shared_ptr<variable_entity> as_variable() { return nullptr; }
	virtual std::shared_ptr<function_entity> as_function() { return nullptr; }
	virtual std::shared_ptr<type_entity> as_type() { return nullptr; }
	virtual std::shared_ptr<module_entity> as_module() { return nullptr; }
	virtual std::shared_ptr<generic_param_entity> as_generic_param() { return nullptr; }

protected:
	entity(entity_kind kind)
		: e_kind(kind) {
	}

private:
	entity_kind e_kind;
	entity_id id;

	// may change this in the future
	// if more classes need to set the id
	// but thats fairly unlikely
	friend class symbol_table;
};