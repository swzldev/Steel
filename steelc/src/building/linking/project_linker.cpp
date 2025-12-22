#include "project_linker.h"

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>

// -- linker backend implementations --
#include <linking_llvm/llvm_code_linker.h>

static std::map<std::string, std::shared_ptr<icode_linker>> get_linkers() {
	static const std::map<std::string, std::shared_ptr<icode_linker>> linkers = {
		{ "llvm", std::make_shared<llvm_code_linker>() }
	};

	return linkers;
}

code_artifact project_linker::link_all(icode_linker* linker) {
	if (linker) {
		return linker->link(link_artifacts, cfg);
	}
	return get_linker()->link(link_artifacts, cfg);
}

icode_linker* project_linker::get_linker() const {
	auto linkers = get_linkers();
	auto it = linkers.find(cfg.backend);
	if (it == linkers.end()) {
		throw std::runtime_error("No linker found for backend: " + cfg.backend);
	}
	return it->second.get();
}
