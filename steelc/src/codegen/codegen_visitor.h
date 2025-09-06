#pragma once

#include <string>
#include <map>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include "../parser/ast/ast_visitor.h"

class codegen_visitor : public ast_visitor {
public:
	codegen_visitor() = default;

	void visit(std::shared_ptr<binary_expression> expr) override;
	void visit(std::shared_ptr<identifier_expression> id) override;
	void visit(std::shared_ptr<literal> literal) override;

private:
	llvm::LLVMContext context;
	llvm::IRBuilder<> builder;
	llvm::Value* last_value;

	llvm::Value* generate_literal(std::shared_ptr<literal> lit);
};