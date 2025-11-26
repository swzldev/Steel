#pragma once

#include <utility>

template<typename T>
class maybe {
public:
	maybe()
		: has_value(false) {
	}
	maybe(const T& value)
		: has_value(true), val(value) {
	}
	maybe(T&& value)
		: has_value(true), val(std::move(value)) {
	}

	inline bool value() const {
		return has_value;
	}
	inline T& get() {
		return value;
	}
	inline const T& get() const {
		return value;
	}

private:
	bool has_value;
	T val;
};