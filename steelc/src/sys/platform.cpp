#include "platform.h"

#include <string>
#include <unordered_map>
#include <algorithm>

#include <sys/host_defs.h>

platform platform::host_platform() {
#if defined(STEELC_ARCH_X64)
	platform_arch arch = platform_arch::X64;
#elif defined(STEELC_ARCH_X86)
	platform_arch arch = platform_arch::X86;
#elif defined(STEELC_ARCH_ARM64)
	platform_arch arch = platform_arch::ARM64;
#elif defined(STEELC_ARCH_ARM)
	platform_arch arch = platform_arch::ARM;
#endif

#if defined(STEELC_PLATFORM_WINDOWS)
	platform_os os = platform_os::WINDOWS;
	platform_abi abi = platform_abi::MSVC;
	platform_vendor vendor = platform_vendor::PC;
#elif defined(STEELC_PLATFORM_LINUX)
	platform_os os = platform_os::LINUX;
	platform_abi abi = platform_abi::GNU;
	platform_vendor vendor = platform_vendor::UNKNOWN;
#elif defined(STEELC_PLATFORM_OSX)
	platform_os os = platform_os::OSX;
	platform_abi abi = platform_abi::APPLE;
	platform_vendor vendor = platform_vendor::APPLE;
#endif

	return platform(arch, vendor, os, abi);
}

platform_arch platform::parse_arch(const std::string& str) {
	auto arch_map = get_arch_map();

	std::string lower;
	lower.resize(str.size());
	std::transform(str.begin(), str.end(), lower.begin(), std::tolower);

	auto it = arch_map.find(str);
	if (it != arch_map.end()) {
		return it->second;
	}
	return platform_arch::UNKNOWN;
}
platform_vendor platform::parse_vendor(const std::string& str) {
	auto arch_map = get_vendor_map();

	std::string lower;
	lower.resize(str.size());
	std::transform(str.begin(), str.end(), lower.begin(), std::tolower);

	auto it = arch_map.find(str);
	if (it != arch_map.end()) {
		return it->second;
	}
	return platform_vendor::UNKNOWN;
}
platform_os platform::parse_os(const std::string& str) {
	auto os_map = get_os_map();

	std::string lower;
	lower.resize(str.size());
	std::transform(str.begin(), str.end(), lower.begin(), std::tolower);

	auto it = os_map.find(lower);
	if (it != os_map.end()) {
		return it->second;
	}
	return platform_os::UNKNOWN;
}
platform_abi platform::parse_abi(const std::string& str) {
	auto abi_map = get_abi_map();

	std::string lower;
	lower.resize(str.size());
	std::transform(str.begin(), str.end(), lower.begin(), std::tolower);

	auto it = abi_map.find(lower);
	if (it != abi_map.end()) {
		return it->second;
	}
	return platform_abi::UNKNOWN;
}

std::string platform::to_string(platform_arch arch) {
	if (arch == platform_arch::UNKNOWN) {
		return "unknown";
	}
	for (const auto& pair : get_arch_map()) {
		if (pair.second == arch) {
			return pair.first;
		}
	}
}
std::string platform::to_string(platform_vendor vendor) {
	if (vendor == platform_vendor::UNKNOWN) {
		return "unknown";
	}
	for (const auto& pair : get_vendor_map()) {
		if (pair.second == vendor) {
			return pair.first;
		}
	}
}
std::string platform::to_string(platform_os os) {
	if (os == platform_os::UNKNOWN) {
		return "unknown";
	}
	for (const auto& pair : get_os_map()) {
		if (pair.second == os) {
			return pair.first;
		}
	}
}
std::string platform::to_string(platform_abi abi) {
	if (abi == platform_abi::UNKNOWN) {
		return "unknown";
	}
	for (const auto& pair : get_abi_map()) {
		if (pair.second == abi) {
			return pair.first;
		}
	}
}

std::unordered_map<std::string, platform_arch> platform::get_arch_map() {
	return {
		// x86
		{ "x86", platform_arch::X86 },
		{ "i386", platform_arch::X86 },
		{ "i486", platform_arch::X86 },
		{ "i586", platform_arch::X86 },
		{ "i686", platform_arch::X86 },
		{ "i786", platform_arch::X86 },
		{ "ia32", platform_arch::X86 },

		// x64
		{ "x64", platform_arch::X64 },
		{ "amd64", platform_arch::X64 },
		{ "x86-64", platform_arch::X64 },
		{ "x86_64", platform_arch::X64 },

		// arm
		{ "arm", platform_arch::ARM },
		{ "arm32", platform_arch::ARM },
		{ "armhf", platform_arch::ARM },
		{ "armv6", platform_arch::ARM },
		{ "armv6l", platform_arch::ARM },
		{ "armv7", platform_arch::ARM },
		{ "armv7l", platform_arch::ARM },

		// arm64
		{ "arm64", platform_arch::ARM64 },
		{ "aarch64", platform_arch::ARM64 },
	};
}
std::unordered_map<std::string, platform_vendor> platform::get_vendor_map() {
	return {
		{ "pc", platform_vendor::PC },
		{ "apple", platform_vendor::APPLE },
	};
}
std::unordered_map<std::string, platform_os> platform::get_os_map() {
	return {
		// windows
		{ "windows", platform_os::WINDOWS },
		{ "win32", platform_os::WINDOWS },
		{ "win64", platform_os::WINDOWS },

		// linux
		{ "linux", platform_os::LINUX },

		// osx
		{ "osx", platform_os::OSX },
		{ "macos", platform_os::OSX },
	};
}
std::unordered_map<std::string, platform_abi> platform::get_abi_map() {
	return {
		// GNU libc (Linux x86/x86-64)
		{ "gnu", platform_abi::GNU },

		// GNU libc - ARM32 variants
		{ "gnueabi", platform_abi::ARM_EABI_SOFT },
		{ "gnueabihf", platform_abi::ARM_EABI_HARD },

		// MUSL libc (Linux x86/x86-64)
		{ "musl", platform_abi::MUSL },

		// MUSL libc - ARM32 variants
		{ "musleabi", platform_abi::ARM_EABI_SOFT },
		{ "musleabihf", platform_abi::ARM_EABI_HARD },

		// MSVC (Windows)
		{ "msvc", platform_abi::MSVC },

		// Apple / Darwin
		{ "apple", platform_abi::APPLE },
		{ "darwin", platform_abi::APPLE },

		// BSD variants
		{ "freebsd", platform_abi::BSD },
		{ "openbsd", platform_abi::BSD },
		{ "netbsd", platform_abi::BSD },

		// ARM64 / AArch64
		{ "aarch64", platform_abi::ARM64_AAPCS },
		{ "aarch64_be", platform_abi::ARM64_AAPCS },
	};
}
