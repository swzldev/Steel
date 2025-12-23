#pragma once

#include <cstdint>
#include <string>

#include <mir/mir_type.h>

// mir_value
//
// represents a value in MIR such as a variable or temporary

class mir_value {
public:
	using id_type = int64_t;

public:
	mir_value() = default;
	mir_value(id_type id, const mir_type& ty, const std::string& name = "")
		: id(id), type(ty), name(name) {
	}


	inline bool valid() const {
		return id != -1;
	}

	inline id_type get_id() const {
		return id;
	}
	inline mir_type get_type() const {
		return type;
	}
	inline std::string get_name() const {
		return name;
	}

private:
	id_type id = -1;
	mir_type type{};
	std::string name; // for debugging
};
