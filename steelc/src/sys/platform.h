#pragma once

#include <string>

// platform
//
// defines platform specific constants and enumerations
// as well as helper functions for working with platform specifics

enum class platform_arch {
	UNKNOWN,

	X86,
	X64,
	ARM,
	ARM64,
};

enum class platform_os {
	UNKNOWN,

	LINUX,
	WINDOWS,
	OSX,
};

enum class platform_abi {
	UNKNOWN,

	GNU,
	MUSL,
	MSVC,
};

class platform {
public:
	platform(platform_arch arch, platform_os os, platform_abi abi)
		: arch(arch), os(os), abi(abi) {
	}

	static platform host_platform();

	static platform_arch parse_arch(const std::string& str);
	static platform_os parse_os(const std::string& str);
	static platform_abi parse_abi(const std::string& str);

	platform_arch arch;
	platform_os os;
	platform_abi abi;
};