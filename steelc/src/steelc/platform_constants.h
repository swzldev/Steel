#pragma once

// this file defines platform constants useful throughout the codebase

#if defined(_WIN32) || defined(_WIN64)
#define STEELC_PLATFORM_WINDOWS 1

#elif defined(__linux__)
#define STEELC_PLATFORM_LINUX 1

#elif defined(__APPLE__) && defined(__MACH__)
#define STEELC_PLATFORM_OSX 1

#else
#error "Unknown platform"
#endif


#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define STEELC_ARCH_X64 1

#elif defined(_M_IX86) || defined(__i386__)
#define STEELC_ARCH_X86 1

#elif defined(_M_ARM64) || defined(__aarch64__)
#define STEELC_ARCH_ARM64 1

#elif defined(_M_ARM) || defined(__arm__)
#define STEELC_ARCH_ARM 1

#else
#error "Unknown architecture"
#endif