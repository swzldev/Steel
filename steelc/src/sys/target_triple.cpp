#include "target_triple.h"

#include <string>
#include <vector>
#include <sstream>

#include <sys/platform.h>

target_triple::target_triple(const std::string& triple_str) {
	string = triple_str;
	parse(triple_str);
}

void target_triple::parse(const std::string& triple_str) {
	std::vector<std::string> components;

	std::istringstream ss(triple_str);
	std::string item;
	while (std::getline(ss, item, '-')) {
		components.push_back(item);
	}

	if (components.empty()) return; // nothing to parse
	size_t num_components = components.size();

	// force first to be arch
	if (num_components >= 1) {
		arch_str = components[0];
		arch_cache = platform::parse_arch(components[0]);
	}

	// for the rest we can just attempt to parse
	// and set if successful
	for (size_t i = 0; i < num_components; i++) {
		// priority: os -> abi -> vendor
		if (auto os = platform::parse_os(components[i]); os != platform_os::UNKNOWN) {
			os_str = components[i];
			os_cache = os;
		}
		else if (auto abi = platform::parse_abi(components[i]); abi != platform_abi::UNKNOWN) {
			abi_str = components[i];
			abi_cache = abi;
		}
		// vendor is not really necessary so for now we
		// can ignore it
	}
}
