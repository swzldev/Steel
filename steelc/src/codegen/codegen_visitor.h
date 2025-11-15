#pragma once

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include "../parser/ast/ast_visitor.h"

class codegen_visitor : public ast_visitor {
public:
	codegen_visitor() = default;

	void visit(std::shared_ptr<function_declaration> func);

private:
	llvm::LLVMContext context;
	llvm::Module* module;
	llvm::IRBuilder<> builder;
	llvm::Value* result;

	llvm::Value* generate_literal(std::shared_ptr<literal> lit);
};