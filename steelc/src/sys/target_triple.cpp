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

	if (components.size() >= 1) {
		arch_str = components[0];
		arch_cache = platform::parse_arch(components[0]);
	}

	if (components.size() >= 2) {
		vendor_str = components[1];
		vendor_cache = platform::parse_vendor(components[1]);
	}

	if (components.size() >= 3) {
		os_str = components[2];
		os_cache = platform::parse_os(components[2]);
	}

	if (components.size() >= 4) {
		abi_str = components[3];
		abi_cache = platform::parse_abi(components[3]);
	}
}

target_triple target_triple::host_triple() {
	platform host = platform::host_platform();

	// construct triple string from host platform
	std::string triple_str = platform::to_string(host.arch) + "-"
		+ platform::to_string(host.vendor) + "-"
		+ platform::to_string(host.os) + "-"
		+ platform::to_string(host.abi);
	return target_triple(triple_str);
}
