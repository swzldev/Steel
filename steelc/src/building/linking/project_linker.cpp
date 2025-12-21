#include "project_linker.h"

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>

// -- linker backend implementations --
#include <linking_llvm/llvm_code_linker.h>

code_artifact project_linker::link_all(icode_linker* linker) {
	if (linker) {
		return linker->link(link_artifacts);
	}
	return get_linker()->link(link_artifacts);
}

icode_linker* project_linker::get_linker() {
	if (linker) return linker.get();

	switch (cfg.backend) {
	case codegen_backend::LLVM:
		linker = std::make_unique<llvm_code_linker>();
		break;
	default:
		throw std::runtime_error("Unsupported codegen backend");
	}
	return linker.get();
}
