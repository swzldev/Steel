#pragma once

#include <vector>
#include <string>
#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "cleanup/scope.h"
#include "cleanup/cleanup_action.h"
#include "memory/lvalue.h"
#include "memory/variable.h"
#include "../parser/ast/expressions/expression.h"

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

	// create a new block in the current function
	llvm::BasicBlock* make_block(const std::string& name, bool append_to_func = true);

	// creates a new scope
	void enter_scope();
	// leaves the current scope, emitting cleanup code INLINE
	void leave_scope_inl();
	// register a cleanup action that will occur when leaving the current scope
	void register_cleanup(const cleanup_action& action);

	// initializes the return slot (for non-void functions)
	void alloc_return_slot(llvm::Type* ret_type);

	// allocates a local variable
	std::shared_ptr<variable> alloc_local(llvm::Type* type, const std::string& name);
	// lookup a local variable by name in the current scope stack (from innermost to outermost)
	std::shared_ptr<variable> lookup_local(const std::string& name);

	std::shared_ptr<lvalue> lvalue_from_expression(std::shared_ptr<expression> expr);

	// emits a lifetime.start call
	void emit_life_start(llvm::Value* ptr);
	void emit_return(llvm::Value* ret_value = nullptr);
	void build_cleanup_chain();
	void finalize_function();

private:
	llvm::AllocaInst* create_alloca(llvm::Type* type, const std::string& name = "");
	void emit_cleanup(const cleanup_action& action);
};