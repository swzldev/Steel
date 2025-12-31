#pragma once

#include <string>

enum class operating_system {
	AUTO, // auto-detect from host

	LINUX,
	WINDOWS,
	OSX,
};

namespace system_formats {
	// constants
	constexpr auto COFF = "COFF";
	constexpr auto ELF = "ELF";
	constexpr auto MACH_O = "Mach-O";

	// utility functions
	std::string get_object_format(operating_system os = operating_system::AUTO);
	std::string get_object_extension(operating_system os = operating_system::AUTO);
}