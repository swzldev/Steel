#include "codegen.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

#include <ast/compilation_unit.h>
#include <codegen/error/codegen_exception.h>
#include <codegen/codegen_result.h>
#include <codegen/icode_generator.h>
#include <output/output.h>

// -- code generator backend implementations --
#include <codegen_llvm/llvm_code_generator.h>

static std::map<std::string, std::shared_ptr<icode_generator>> get_generators() {
	static const std::map<std::string, std::shared_ptr<icode_generator>> generators = {
		{ "llvm", std::make_shared<llvm_code_generator>() }
	};

	return generators;
}

bool codegen::validate_backend(const std::string& backend) {
	auto gens = get_generators();
	return gens.find(backend) != gens.end();
}
bool codegen::backend_info(const std::string& backend, codegen_info* result) {
	auto gens = get_generators();
	auto it = gens.find(backend);
	if (it != gens.end()) {
		*result = it->second->info();
		return true;
	}
	return false;
}

codegen_result codegen::generate(size_t index) {
	auto& mod = mir_modules[index];
	codegen_result result;
	try {
		auto res = get_generator()->emit(mod, cfg);
		result.merge(res);
	}
	catch (const codegen_exception& e) {
		output::err("Codegen error in module {}: {}\n", console_colors::RED, mod.name, e.message());
		result.success = false;
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

icode_generator* codegen::get_generator() const {
	auto gens = get_generators();
	auto it = gens.find(cfg.backend);
	if (it != gens.end()) {
		return it->second.get();
	}
	return nullptr;
}
