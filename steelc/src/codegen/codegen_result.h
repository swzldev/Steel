#pragma once

#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <codegen/modules/module_holder.h>

class codegen_result {
public:
	// move only
	codegen_result(const codegen_result&) = delete;
	codegen_result& operator=(const codegen_result&) = delete;
	codegen_result(codegen_result&&) = default;
	codegen_result& operator=(codegen_result&&) = default;

public:
	codegen_result()
		: context(nullptr) {
	}
	codegen_result(std::unique_ptr<llvm::LLVMContext> ctx)
		: context(std::move(ctx)) {
	}

	std::unique_ptr<llvm::LLVMContext> context;
	std::vector<module_holder> modules;
};