#include "system_formats.h"

#include <string>

#include <steelc/platform_constants.h>

static inline operating_system get_host_os() {
#if defined(STEELC_PLATFORM_LINUX)
	return operating_system::LINUX;
#elif defined(STEELC_PLATFORM_WINDOWS)
	return operating_system::WINDOWS;
#elif defined(STEELC_PLATFORM_OSX)
	return operating_system::OSX;
#else
  #error "Could not detect host operating system"
#endif
}

std::string system_formats::get_object_format(operating_system os) {
	if (os == operating_system::AUTO) {
		os = get_host_os();
	}

	switch (os) {
	case operating_system::LINUX:
		return ELF;
	case operating_system::WINDOWS:
		return COFF;
	case operating_system::OSX:
		return MACH_O;
	default:
		return "Unknown";
	}
}
std::string system_formats::get_object_extension(operating_system os) {
	if (os == operating_system::AUTO) {
		os = get_host_os();
	}

	switch (os) {
	case operating_system::LINUX:
		return ".o";
	case operating_system::WINDOWS:
		return ".obj";
	case operating_system::OSX:
		return ".o";
	default:
		return "";
	}
}
