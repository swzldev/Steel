#pragma once

#include <vector>
#include <string>

#include <representations/entities/entities_fwd.h>

class lookup_result {
public:
	lookup_result() = default;
	lookup_result(std::vector<entity_ptr> results)
		: results(results) {
	}
	lookup_result(entity_ptr ent)
		: results({ ent }) {
	}

	// found at least 1 result
	// check for collisions BEFORE calling
	// as it will still return true for collisions!
	inline bool found() const {
		return !results.empty();
	}
	// found no results
	inline bool no_match() const {
		return results.empty();
	}
	// found multiple results
	inline bool ambiguous() const {
		return results.size() > 1;
	}
	// get the first result (or or null if none)
	inline entity_ptr first() const {
		if (results.empty()) {
			return nullptr;
		}
		return results[0];
	}

	std::vector<std::string> results_names(bool full_names = true);

	std::vector<entity_ptr> results = {};
};