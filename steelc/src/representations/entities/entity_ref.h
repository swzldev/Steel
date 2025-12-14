#pragma once

#include <memory>

#include <representations/entities/entities_fwd.h>

class entity_ref {
public:
	entity_ref()
		: id(-1) {
	}

	entity_ptr get() const;

private:
	entity_id id;
};