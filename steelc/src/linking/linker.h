#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>
#include <linking/link_result.h>

class linker {
public:
	linker(const std::vector<std::string>& to_link_paths, const codegen_config& cfg, const std::string& obj_format)
		: to_link_paths(to_link_paths), cfg(cfg), obj_format(obj_format) {
	}

	// nullptr for default linker (based on cfg)
	link_result link_all(icode_linker* linker = nullptr);

	// always check a linker is available before calling link_all
	bool linker_available() const;

private:
	std::vector<std::string> to_link_paths;
	codegen_config cfg;
	std::string obj_format;

	icode_linker* get_linker() const;
};