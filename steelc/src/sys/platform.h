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

	// Linux / Unix
	GNU,            // glibc, x86/x86-64, general ARM32 soft/hard float handled separately
	MUSL,           // musl libc
	UCLIBC,         // uClibc

	// Windows
	MSVC,           // Microsoft Visual C++

	// Apple platforms
	APPLE,          // macOS, iOS, Darwin

	// BSD variants
	BSD,            // FreeBSD, OpenBSD, NetBSD

	// ARM32
	ARM_EABI_SOFT,  // soft-float
	ARM_EABI_HARD,  // hard-float

	// ARM64
	ARM64_AAPCS,    // 64-bit ARM, always hard-float
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