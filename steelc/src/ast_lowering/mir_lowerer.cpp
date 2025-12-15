#include "mir_lowerer.h"

#include <ast/ast.h>
#include <mir/mir_module.h>
#include <mir/mir_function.h>

mir_module mir_lowerer::lower_unit(std::shared_ptr<compilation_unit> unit) {
	mir_module module;
	for (auto& func : unit->declarations) {

	}
}
