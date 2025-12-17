#pragma once

#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>

// mir_type
//
// represents a language type, used in MIR instructions and operands
// currently this is just a wrapper around type_ptr

struct mir_type {
	type_ptr ty; // change this in the future

	bool operator ==(const mir_type& other) const {
		return *ty == other.ty;
	}
};