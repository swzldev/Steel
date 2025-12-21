#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

// code_artifact
//
// represents a generated code artifact from code generation
// e.g., object file, bitcode file, source file, etc.
//
// formats:
// - "LLVM-IR": LLVM IR assembly representation (text)
// - "LLVM-BC": LLVM IR bitcode representation (binary)

enum artifact_kind {
	ARTIFACT_SOURCE,		// source file (e.g., .c, .cpp, .rs, etc.)
	ARTIFACT_IR,			// intermediate representation (e.g., .ll, .bc, .obj)
	ARTIFACT_LIBRARY,		// static or dynamic library (e.g., .lib, .a, .dll, .so)
	ARTIFACT_BINARY,		// final executable binary (e.g., .exe, no extension, etc.)
	ARTIFACT_META,			// metadata file (e.g., .json, .xml, etc.)
};

struct code_artifact {
	artifact_kind kind;
	std::string src_relpath;									// relative path of source that generated this artifact
	std::string name;											// logical name (no path)
	std::string extension;										// e.g., ".o", ".bc", ".ll", etc.
	std::string format;											// e.g., "ELF", "COFF", "LLVM-IR", etc.
	std::unordered_map<std::string, std::string> attributes;	// target triple, cpu, etc.
	bool is_binary = true;										// true if binary data, false if text data
	std::vector<uint8_t> bytes;									// for binary data
	std::string text;											// for text data
};