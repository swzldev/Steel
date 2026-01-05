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
#elif defined(STEELC_PLATFORM_LINUX)
	platform_os os = platform_os::LINUX;
#elif defined(STEELC_PLATFORM_OSX)
	platform_os os = platform_os::OSX;
#endif

	return platform(arch, os, platform_abi::UNKNOWN);
}

platform_arch platform::parse_arch(const std::string& str) {
	static const std::unordered_map<std::string, platform_arch> arch_map = {
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

	std::string lower;
	lower.resize(str.size());
	std::transform(str.begin(), str.end(), lower.begin(), std::tolower);

	auto it = arch_map.find(str);
	if (it != arch_map.end()) {
		return it->second;
	}
	return platform_arch::UNKNOWN;
}
platform_os platform::parse_os(const std::string& str) {
	static const std::unordered_map<std::string, platform_os> os_map = {
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
	static const std::unordered_map<std::string, platform_abi> abi_map = {
		// gnu
		{ "gnu", platform_abi::GNU },
		{ "gnueabi", platform_abi::GNU },
		{ "gnueabihf", platform_abi::GNU },

		// musl
		{ "musl", platform_abi::MUSL },
		{ "musleabi", platform_abi::MUSL },
		{ "musleabihf", platform_abi::MUSL },

		// msvc
		{ "msvc", platform_abi::MSVC },
	};

	std::string lower;
	lower.resize(str.size());
	std::transform(str.begin(), str.end(), lower.begin(), std::tolower);

	auto it = abi_map.find(lower);
	if (it != abi_map.end()) {
		return it->second;
	}
	return platform_abi::UNKNOWN;
}
