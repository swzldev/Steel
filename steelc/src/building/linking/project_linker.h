#pragma once

#include <vector>
#include <memory>
#include <string>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>
#include <building/linking/link_error.h>

class project_linker {
public:
	project_linker() = default;
	project_linker(const std::vector<code_artifact>& link_artifacts, const codegen_config& cfg)
		: link_artifacts(link_artifacts), cfg(cfg) {
	}

	code_artifact link_all(icode_linker* linker) const;

	inline bool has_error() const {
		return last_error != LINK_OK;
	}
	inline link_error get_last_error() const {
		return last_error;
	}
	inline std::string get_error_message() const {
		return error_message;
	}

private:
	std::vector<code_artifact> link_artifacts;
	codegen_config cfg;

	link_error last_error = LINK_OK;
	std::string error_message;

	inline void error(link_error err, const std::string& msg) {
		last_error = err;
		error_message = msg;
	}
};