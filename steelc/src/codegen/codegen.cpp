#include "codegen.h"

#include <string>
#include <vector>
#include <memory>

#include <ast/compilation_unit.h>
#include <codegen/error/codegen_exception.h>
#include <codegen/codegen_result.h>
#include <codegen/icode_generator.h>
#include <output/output.h>

// -- code generator backend implementations --
#include <codegen_llvm/llvm_code_generator.h>

codegen_result codegen::generate(size_t index) {
	auto& mod = mir_modules[index];
	codegen_result result;
	try {
		auto res = get_generator()->emit(mod, cfg);
		result.merge(res);
	}
	catch (const codegen_exception& e) {
		output::err("Codegen error in module {}: {}\n", console_colors::RED, mod.name, e.message());
	}
	return result;
}
codegen_result codegen::generate_all() {
	codegen_result result;

	for (size_t i = 0; i < mir_modules.size(); i++) {
		auto res = generate(i);
		result.merge(res);
	}

	return result;
}

icode_generator* codegen::get_generator() {
	if (generator) return generator.get();
	switch (cfg.backend) {
	case codegen_backend::LLVM:
		generator = std::make_unique<llvm_code_generator>();
		break;
	default:
		throw codegen_exception("Unsupported codegen backend");
	}
	return generator.get();
}
