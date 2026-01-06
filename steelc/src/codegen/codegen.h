#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <memory>

#include <codegen/codegen_result.h>
#include <codegen/codegen_config.h>
#include <codegen/icode_generator.h>
#include <mir/mir_fwd.h>

// codegen
//
// this code handles generating low level code from MIR
// it does not produce or generate any code, this is provided
// by the icode_generator passed (usually the llvm_code_generator)

class codegen {
public:
	codegen(const std::vector<mir_module>& mir_modules, const codegen_config& cfg)
		: mir_modules(mir_modules), cfg(cfg) {
	}

	static bool validate_backend(const std::string& backend);
	static bool backend_info(const std::string& backend, codegen_info* result);

	codegen_result generate(size_t index);
	codegen_result generate_all();

private:
	std::vector<mir_module> mir_modules;
	codegen_config cfg;

	icode_generator* get_generator() const;
};