#pragma once

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <ast/compilation_unit.h>

class module_holder {
public:
	// move only (due to unique_ptr)
	module_holder(const module_holder&) = delete;
	module_holder& operator=(const module_holder&) = delete;
	module_holder(module_holder&&) = default;
	module_holder& operator=(module_holder&&) = default;

public:
	module_holder(const std::string& mod_name, llvm::LLVMContext& ctx, std::shared_ptr<compilation_unit> unit)
		: module(std::make_unique<llvm::Module>(mod_name, ctx)), owning_unit(unit) {
	}
	module_holder(std::unique_ptr<llvm::Module> module)
		: module(std::move(module)), owning_unit(nullptr) {
	}

	std::unique_ptr<llvm::Module> module;
	std::shared_ptr<compilation_unit> owning_unit; // nullptr for linked modules
};