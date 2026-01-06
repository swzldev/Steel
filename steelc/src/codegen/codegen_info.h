#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include <sys/platform.h>

// codegen_info
//
// this type holds information about a specific code generator
// it is returned by the code generator and used to query its capabilities
// and properties

enum class codegen_capability {
	// can generate intermediate representation files
	CAN_GENERATE_IR,
	// can generate native object files
	CAN_GENERATE_NATIVE_OBJECT,
};

class codegen_info {
public:
	inline bool capable_of(codegen_capability cap) const {
		return contains(capabilities, cap);
	}

	inline bool supports_arch(platform_arch arch) const {
		return contains(supported_architectures, arch);
	}
	inline bool supports_os(platform_os os) const {
		return contains(supported_oses, os);
	}
	inline bool supports_abi(platform_abi abi) const {
		return contains(supported_abis, abi);
	}

	inline bool supports_ir_format(const std::string& format) const {
		return contains(supported_ir_formats, format);
	}

	std::string name;
	std::string description;

	std::vector<codegen_capability> capabilities;

	std::vector<platform_arch> supported_architectures;
	std::vector<platform_os> supported_oses;
	std::vector<platform_abi> supported_abis;

	// note: supported_ir_formats[0] is considered the default IR format
	// empty and unused if !capable_of(CAN_GENERATE_IR)
	std::vector<std::string> supported_ir_formats;

private:
	template<typename T>
	inline bool contains(const std::vector<T>& vec, T val) const {
		return std::find(vec.begin(), vec.end(), val) != vec.end();
	}
};