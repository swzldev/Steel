#include "lookup_result.h"

#include <vector>
#include <string>

#include <representations/entities/entity.h>

std::vector<std::string> lookup_result::results_names(bool full_names) {
	std::vector<std::string> names;

	for (const auto& res : results) {
		if (full_names) {
			names.push_back(res->full_name());
		}
		else {
			names.push_back(res->name());
		}
	}

	return names;
}