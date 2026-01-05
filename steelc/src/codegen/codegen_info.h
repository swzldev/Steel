#pragma once

#include <string>
#include <vector>

#include <sys/platform.h>

// codegen_info
//
// this type holds information about a specific code generator
// it is returned by the code generator and used to query its capabilities
// and properties

enum class codegen_capability {
	// can generate native object files
	CAN_GENERATE_NATIVE_OBJECT,
};

enum class codegen_output_kind {
	NATIVE_OBJECT,
};

class codegen_info {
public:
	std::vector<codegen_capability> capabilities;
	std::vector<codegen_output_kind> output_kinds;

	std::vector<platform_arch> supported_architectures;
	std::vector<platform_os> supported_oses;
};