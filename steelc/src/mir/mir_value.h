#pragma once

#include <cstdint>
#include <string>

#include <mir/mir_type.h>

// mir_value
//
// represents a value in MIR such as a variable or temporary

class mir_value {
public:
	mir_value() = default;
	mir_value(int64_t id, const mir_type& ty, const std::string& name = "")
		: id(id), type(ty), name(name) {
	}

	inline bool valid() const {
		return id != -1;
	}

	inline int64_t get_id() const {
		return id;
	}
	inline mir_type get_type() const {
		return type;
	}
	inline std::string get_name() const {
		return name;
	}

private:
	int64_t id = -1;
	mir_type type{};
	std::string name; // for debugging
};
