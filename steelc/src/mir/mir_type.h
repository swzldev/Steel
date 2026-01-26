#pragma once

#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>

// mir_type
//
// represents a language type, used in MIR instructions and operands
// since we already have a robust type system thats constructed during
// semantic analysis, we can just use that here

struct mir_type {
	type_ptr ty;

	bool operator ==(const mir_type& other) const {
		return *ty == other.ty;
	}
};
