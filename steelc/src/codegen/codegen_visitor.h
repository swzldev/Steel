#pragma once

#include <memory>
#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include "codegen_env.h"
#include "type_handling/llvm_type_converter.h"
#include "builders/llvm_function_builder.h"
#include "builders/llvm_expression_builder.h"
#include "../lexer/token_type.h"
#include "../parser/ast/ast_visitor.h"
#include "../parser/ast/ast_fwd.h"

class codegen_visitor : public ast_visitor {
public:
	codegen_visitor(std::string unit_name)
		: module(unit_name, context),
		builder(context),
		type_converter(context),
		function_builder(module, type_converter),
		expression_builder(builder),
		result(nullptr) {
	}

	void visit(std::shared_ptr<function_declaration> func) override;
	void visit(std::shared_ptr<variable_declaration> var) override;
	//void visit(std::shared_ptr<type_declaration> decl);
	//void visit(std::shared_ptr<module_declaration> mod);
	//void visit(std::shared_ptr<conversion_declaration> conv);
	//void visit(std::shared_ptr<operator_declaration> op);
	//void visit(std::shared_ptr<enum_declaration> enum_decl);
	//void visit(std::shared_ptr<enum_option> option);
	//void visit(std::shared_ptr<expression_statement> expr);
	void visit(std::shared_ptr<binary_expression> expr);
	void visit(std::shared_ptr<assignment_expression> expr);
	//void visit(std::shared_ptr<member_expression> expr);
	void visit(std::shared_ptr<address_of_expression> expr);
	void visit(std::shared_ptr<deref_expression> expr);
	void visit(std::shared_ptr<unary_expression> expr);
	void visit(std::shared_ptr<index_expression> expr);
	void visit(std::shared_ptr<identifier_expression> id) override;
	//void visit(std::shared_ptr<this_expression> expr);
	//void visit(std::shared_ptr<cast_expression> expr);
	//void visit(std::shared_ptr<initializer_list> init);
	void visit(std::shared_ptr<function_call> func_call);
	void visit(std::shared_ptr<literal> literal) override;
	//virtual void visit(std::shared_ptr<import_statement> import_stmt);
	void visit(std::shared_ptr<code_block> block) override;
	void visit(std::shared_ptr<if_statement> if_stmt) override;
	//void visit(std::shared_ptr<inline_if> inline_if);
	void visit(std::shared_ptr<for_loop> for_loop);
	void visit(std::shared_ptr<while_loop> while_loop);
	void visit(std::shared_ptr<return_statement> ret_stmt) override;
	void visit(std::shared_ptr<break_statement> brk_stmt);
	
	llvm::Module& get_module() {
		return module;
	}

private:
	llvm::LLVMContext context;
	llvm::Module module;
	llvm::IRBuilder<> builder;
	// with the new accept method, NEVER directly use this field
	// it should ONLY be set
	llvm::Value* result;

	llvm_type_converter type_converter;
	llvm_function_builder function_builder;
	llvm_expression_builder expression_builder;

	std::unique_ptr<codegen_env> env;

	llvm::Value* generate_literal(std::shared_ptr<literal> lit);

	bool op_requires_lvalue(token_type op);

public:
	// helper function to accept a node and return the resulting Value*
	// this is much safer as it resets 'result' to nullptr before visiting
	// preventing the accidental use of stale values
	template<typename Nty>
	llvm::Value* accept(std::shared_ptr<Nty>& node);
};

template<typename Nty>
llvm::Value* codegen_visitor::accept(std::shared_ptr<Nty>& node) {
	result = nullptr;
	if (node) node->accept(*this);
	return result;
}