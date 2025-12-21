#pragma once

#include <string>
#include <cstdint>

#include <codegen/code_artifact.h>

// artifact_metadata
//
// metadata about a generated code artifact
// contains all the same info as code_artifact, but without the actual data
// may use it in the future within code_artifact to avoid repeating
// fields

class artifact_metadata {
public:
	artifact_metadata() = default;

	// *RELATIVE* path (including output directory)
	std::string path;
	artifact_kind kind = ARTIFACT_IR;
	std::string src_relpath;									// relative path of source that generated this artifact
	std::string name;											// logical name (no path)
	std::string extension;										// e.g., ".o", ".bc", ".ll", etc.
	std::string format;											// e.g., "ELF", "COFF", "LLVM-IR", etc.
	std::unordered_map<std::string, std::string> attributes;	// target triple, cpu, etc.
	bool is_binary = true;										// true if binary data, false if text data
};