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

	// arch always first
	if (components.size() >= 1) {
		arch_str = components[0];
		arch_cache = platform::parse_arch(components[0]);
	}

	size_t idx = 1;
	auto os = platform::parse_os(components[1]);
	if (os != platform_os::UNKNOWN) {
		// vendor omitted
		vendor_cache = platform::default_vendor_for_os(os);
		vendor_str = "";

		os_cache = os;
		os_str = components[idx];
		idx++;
	}
	else {
		// vendor is present
		vendor_str = components[idx];
		vendor_cache = platform::parse_vendor(components[idx]);
		idx++;

		// also parse os
		if (idx < components.size()) {
			os_str = components[idx];
			os_cache = platform::parse_os(components[idx]);
			idx++;
		}
	}

	// parse abi/env
	if (idx < components.size()) {
		abi_str = components[idx];
		abi_cache = platform::parse_abi(components[idx]);
	}

	finalize();
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

void target_triple::finalize() {
	// fill abi if missing
	if (abi_cache == platform_abi::UNKNOWN && abi_str.empty()) {
		abi_cache = platform::default_abi_for_os(os_cache);
	}
}
