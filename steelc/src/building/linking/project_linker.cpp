#include "project_linker.h"

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <codegen/code_artifact.h>
#include <codegen/codegen_config.h>
#include <linking/icode_linker.h>

code_artifact project_linker::link_all(icode_linker* linker) const {
	return linker->link(link_artifacts);
}
