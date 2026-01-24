#pragma once

#include <string>
#include <vector>

#include <sys/platform.h>

// target_triple
//
// this class wraps a target triple string

class target_triple {
public:
	target_triple() = default;
	explicit target_triple(const std::string& triple_str);

	void parse(const std::string& triple_str);

	inline platform as_platform() const {
		return platform(arch_cache, vendor_cache, os_cache, abi_cache);
	}

	static target_triple host_triple();

	// the underlying triple string
	std::string string;

	// these strings may be empty if the target triple
	// does not specify them
	std::string arch_str;
	std::string vendor_str;
	std::string os_str;
	std::string abi_str;

	inline platform_arch arch() const { return arch_cache; }
	inline platform_vendor vendor() const { return vendor_cache; }
	inline platform_os os() const { return os_cache; }
	inline platform_abi abi() const { return abi_cache; }

private:
	platform_arch arch_cache = platform_arch::UNKNOWN;
	platform_vendor vendor_cache = platform_vendor::UNKNOWN;
	platform_os os_cache = platform_os::UNKNOWN;
	platform_abi abi_cache = platform_abi::UNKNOWN;

	void finalize();
};