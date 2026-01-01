#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>
#include <linking/link_result.h>
#include <linking/link_data.h>

class linker {
public:
	linker(const link_data& data)
		: data(data) {
	}

	// nullptr for default linker (based on cfg)
	link_result link_all(icode_linker* linker = nullptr);

	// always check a linker is available before calling link_all
	bool linker_available() const;

private:
	link_data data;

	icode_linker* get_linker() const;
};