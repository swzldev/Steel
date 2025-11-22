#pragma once

#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>

#include "cleanup/scope.h"

class codegen_env {
public:
	codegen_env(llvm::Module& mod, llvm::Function& func, llvm::IRBuilder<>& builder)
		: module(mod), func(func), ctx(mod.getContext()), builder(builder) {
		// create return block
		ret_block = llvm::BasicBlock::Create(ctx, "return", &func);
	}

	llvm::Module& module;
	llvm::Function& func;
	llvm::LLVMContext& ctx;
	llvm::IRBuilder<>& builder;

	// for cleanup
	std::vector<scope> scope_stack;

	// function return block
	llvm::BasicBlock* ret_block = nullptr;

	// return value slot (for non-void functions)
	llvm::AllocaInst* ret_slot = nullptr;

	// creates a new scope
	void enter_scope();
	// leaves the current scope, emitting cleanup code INLINE
	void leave_scope_inl();
	// register a cleanup action that will occur when leaving the current scope
	void register_cleanup(const cleanup_action& action);

	// initializes the return slot (for non-void functions)
	void alloc_return_slot(llvm::Type* ret_type);

	// allocates a local variable
	llvm::AllocaInst* alloc_local(llvm::Type* type, const std::string& name);
	// registers a local variable onto the current scope's locals map (for lookup)
	// NOTE: this is already called by alloc_local no need to recall
	void register_local(const std::string& name, llvm::AllocaInst* local);
	// lookup a local variable by name in the current scope stack (from innermost to outermost)
	llvm::AllocaInst* lookup_local(const std::string& name);

	// emits a lifetime.start call
	void emit_life_start(llvm::Value* ptr);
	void emit_return(llvm::Value* ret_value = nullptr);
	void build_cleanup_chain();
	void finalize_function();

private:
	void emit_cleanup(const cleanup_action& action);
};