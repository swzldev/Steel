#pragma once

#include <string>

// platform
//
// defines platform specific constants and enumerations
// as well as helper functions for working with platform specifics

enum class platform_arch {
	X86,
	X64,
	ARM,
	ARM64,

	UNKNOWN,
};

enum class platform_os {
	LINUX,
	WINDOWS,
	OSX,

	UNKNOWN,
};

class platform {
public:
	platform(platform_arch arch, platform_os os)
		: arch(arch), os(os) {
	}

	static platform host_platform();

	static platform_arch parse_arch(const std::string& str);
	static platform_os parse_os(const std::string& str);

	platform_arch arch;
	platform_os os;
};