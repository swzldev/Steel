#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

enum artifact_type {
	ARTIFACT_OBJECT,
	ARTIFACT_BITCODE,
	ARTIFACT_ASM_IR,
	ARTIFACT_SOURCE,
	ARTIFACT_LIBRARY,
	ARTIFACT_BINARY,
	ARTIFACT_META,
};

struct code_artifact {
	artifact_type type;
	std::string name;											// logical name (no path)
	std::string format;											// e.g., "ELF", "COFF", "LLVM-IR", etc.
	std::unordered_map<std::string, std::string> attributes;	// target triple, cpu, etc.
	std::vector<uint8_t> bytes;									// for binary data
	std::string text;											// for text data
};