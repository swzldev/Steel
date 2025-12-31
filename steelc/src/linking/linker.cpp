#include "linker.h"

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <map>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <codegen/sys/system_formats.h>
#include <linking/icode_linker.h>
#include <linking/link_result.h>

// -- linker implementations --
#include <linking/impl/coff_linker.h>

static std::map<std::string, std::shared_ptr<icode_linker>> get_linkers() {
	static const std::map<std::string, std::shared_ptr<icode_linker>> linkers = {
		{ system_formats::COFF, std::make_shared<coff_linker>() }
	};

	return linkers;
}

link_result linker::link_all(icode_linker* linker) {
	if (linker) {
		return linker->link(to_link_paths, cfg);
	}
	return get_linker()->link(to_link_paths, cfg);
}

bool linker::linker_available() const {
	return get_linker() != nullptr;
}

icode_linker* linker::get_linker() const {
	auto linkers = get_linkers();

	auto it = linkers.find(obj_format);
	if (it == linkers.end()) {
		return nullptr;
	}
	return it->second.get();
}
