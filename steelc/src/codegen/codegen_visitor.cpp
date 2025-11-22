#include "codegen_visitor.h"

#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Instructions.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include "error/codegen_exception.h"
#include "cleanup/cleanup_action.h"
#include "codegen_env.h"
#include "../parser/ast/declarations/function_declaration.h"
#include "../parser/ast/declarations/variable_declaration.h"
#include "../parser/ast/expressions/binary_expression.h"
#include "../parser/ast/expressions/function_call.h"
#include "../parser/ast/expressions/identifier_expression.h"
#include "../parser/ast/expressions/literal.h"
#include "../parser/ast/statements/block_statement.h"
#include "../parser/ast/statements/control_flow/if_statement.h"
#include "../parser/ast/statements/control_flow/return_statement.h"
#include "../parser/types/data_type.h"

void codegen_visitor::visit(std::shared_ptr<function_declaration> func) { 
	llvm::Function* fn = function_builder.build(func);

	// function entry block
	llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(context, "entry", fn);
	builder.SetInsertPoint(entry_block);

	env = std::make_unique<codegen_env>(module, *fn, builder);
	cg_assert(env != nullptr, "Failed to create codegen environment for function " + func->identifier);
	env->builder.SetInsertPoint(entry_block);

	// allocate return slot if needed
	if (!func->return_type->is_void()) {
		env->alloc_return_slot(type_converter.convert(func->return_type));
	}

	// create initial scope
	env->enter_scope();

	// allocate params
	int i = 0;
	for (auto& arg : fn->args()) {
		llvm::AllocaInst* param_slot = env->alloc_local(arg.getType(), func->parameters[i]->identifier);
		env->builder.CreateStore(&arg, param_slot);
		i++;
	}

	func->body->accept(*this);

	if (func->implicitly_returns) {
		env->emit_return();
	}

	// cleanup
	env->leave_scope_inl();

	env->finalize_function();
}
void codegen_visitor::visit(std::shared_ptr<variable_declaration> var) {
	llvm::Type* var_type = type_converter.convert(var->type);
	llvm::AllocaInst* var_slot = env->alloc_local(var_type, var->identifier);

	cg_assert(var_slot != nullptr, "Failed to allocate local variable " + var->identifier);

	env->emit_life_start(var_slot);
	env->register_cleanup(cleanup_action::create_lifetime_end(var_slot));

	// initialize variable if there is an initializer
	if (var->initializer) {
		var->initializer->accept(*this);
		llvm::Value* init_value = result;
		cg_assert(init_value != nullptr, "Failed to generate initializer for variable " + var->identifier);
		env->builder.CreateStore(init_value, var_slot);
	}

	// TODO: add support for destructors
}
void codegen_visitor::visit(std::shared_ptr<binary_expression> expr) {
	result = nullptr;
	expr->left->accept(*this);
	llvm::Value* lhs = result;

	result = nullptr;
	expr->right->accept(*this);
	llvm::Value* rhs = result;

	cg_assert(lhs != nullptr, "Failed to generate left operand of binary expression");
	cg_assert(rhs != nullptr, "Failed to generate right operand of binary expression");

	result = expression_builder.build_binary_expr(lhs, rhs, expr->oparator);
}
void codegen_visitor::visit(std::shared_ptr<identifier_expression> id) {
	if (id->id_type == IDENTIFIER_VARIABLE) {
		llvm::AllocaInst* var_slot = env->lookup_local(id->identifier);
		cg_assert(var_slot != nullptr, "Undefined variable: " + id->identifier);
		result = env->builder.CreateLoad(var_slot->getAllocatedType(), var_slot);
	}
}
void codegen_visitor::visit(std::shared_ptr<function_call> func_call) {
	std::vector<llvm::Value*> arg_values;

	for (const auto& arg : func_call->args) {
		result = nullptr;
		arg->accept(*this);
		arg_values.push_back(result);
		cg_assert(result != nullptr, "Failed to generate argument for function call: " + func_call->identifier);
	}

	if (func_call->is_method()) {

	}
	else if (func_call->is_constructor()) {

	}
	else {
		if (func_call->identifier == "printf") {
			auto* printf_type = llvm::FunctionType::get(
				llvm::Type::getInt32Ty(context),
				llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context)),
				true
			);

			auto* printf_fn = llvm::cast<llvm::Function>(module.getOrInsertFunction("printf", printf_type).getCallee());

			result = env->builder.CreateCall(printf_fn, arg_values);
		}

		else {
			llvm::Function* callee_fn = module.getFunction(func_call->identifier);
			cg_assert(callee_fn != nullptr, "Undefined function: " + func_call->identifier);
			result = env->builder.CreateCall(callee_fn, arg_values);
		}
	}
}
void codegen_visitor::visit(std::shared_ptr<literal> literal) {
	result = generate_literal(literal);
}
void codegen_visitor::visit(std::shared_ptr<code_block> block) {
	if (!block->is_body) {
		env->enter_scope();
	}
	for (const auto& stmt : block->body) {
		stmt->accept(*this);
	}
	if (!block->is_body) {
		env->leave_scope_inl();
	}
}
void codegen_visitor::visit(std::shared_ptr<if_statement> if_stmt) {
	result = nullptr;
	if_stmt->condition->accept(*this);
	llvm::Value* condition = result;
	cg_assert(condition != nullptr, "Failed to generate condition for if statement");

	auto* then_block = llvm::BasicBlock::Create(context, "if.then", &env->func);
	auto* else_block = llvm::BasicBlock::Create(context, "if.else");
	auto* merge_block = llvm::BasicBlock::Create(context, "if.merge");
	
	if (if_stmt->else_node) {
		builder.CreateCondBr(condition, then_block, else_block);
	}
	else {
		builder.CreateCondBr(condition, then_block, merge_block);
	}

	// generate then block
	builder.SetInsertPoint(then_block);

	env->enter_scope();
	if_stmt->then_block->accept(*this);
	env->leave_scope_inl();

	if (!builder.GetInsertBlock()->getTerminator()) {
		builder.CreateBr(merge_block);
	}

	// generate else block
	if (if_stmt->else_node) {
		env->func.insert(env->func.end(), else_block);
		builder.SetInsertPoint(else_block);

		env->enter_scope();
		if_stmt->else_node->accept(*this);
		env->leave_scope_inl();

		if (!builder.GetInsertBlock()->getTerminator()) {
			builder.CreateBr(merge_block);
		}
	}

	// continue at merge block
	env->func.insert(env->func.end(), merge_block);
	builder.SetInsertPoint(merge_block);
}
void codegen_visitor::visit(std::shared_ptr<return_statement> ret_stmt) {
	llvm::Value* ret_value = nullptr;
	if (ret_stmt->value) {
		ret_stmt->value->accept(*this);
		ret_value = result;
	}

	env->emit_return(ret_value);
}

llvm::Value* codegen_visitor::generate_literal(std::shared_ptr<literal> lit) {
	switch (lit->primitive) {
	case DT_I16: {
		short value = std::stoi(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(16, value, true));
	}
	case DT_I32: {
		int value = std::stoi(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(32, value, true));
	}
	case DT_I64: {
		long long value = std::stoll(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(64, value, true));
	}
	case DT_BOOL: {
		return llvm::ConstantInt::get(context, llvm::APInt(1, lit->value == "true" ? 1 : 0, false));
	}
	case DT_FLOAT: {
		float fvalue = std::stof(lit->value);
		return llvm::ConstantFP::get(context, llvm::APFloat(fvalue));
	}
	case DT_DOUBLE: {
		double dvalue = std::stod(lit->value);
		return llvm::ConstantFP::get(context, llvm::APFloat(dvalue));
	}
	case DT_STRING: {
		return builder.CreateGlobalStringPtr(lit->value);
	}
	case DT_CHAR: {
		char cvalue = lit->value[0];
		return llvm::ConstantInt::get(context, llvm::APInt(8, cvalue, false));
	}
	case DT_BYTE: {
		unsigned char bvalue = static_cast<unsigned char>(std::stoi(lit->value));
		return llvm::ConstantInt::get(context, llvm::APInt(8, bvalue, false));
	}
	}
}
