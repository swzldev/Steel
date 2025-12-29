#pragma once

// this file defines platform constants useful throughout the codebase

#if defined(_WIN32) || defined(_WIN64)
	#define STEELC_PLATFORM_WINDOWS 1
#elif defined(__linux__)
	#define STEELC_PLATFORM_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
	#define STEELC_PLATFORM_OSX 1
#endif