#include "codegen_env.h"

#include <memory>
#include <string>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

#include "error/codegen_exception.h"
#include "cleanup/cleanup_action.h"
#include "cleanup/scope.h"
#include "memory/lvalue.h"
#include "memory/variable.h"
#include "../parser/entities/entities_fwd.h"
#include "../parser/entities/entity.h"
#include "../parser/entities/variable_entity.h"
#include "../parser/ast/expressions/expression.h"
#include "../parser/ast/expressions/identifier_expression.h"

llvm::BasicBlock* codegen_env::make_block(const std::string& name, bool append_to_func) {
	if (append_to_func) {
		return llvm::BasicBlock::Create(ctx, name, &func);
	}
	else {
		return llvm::BasicBlock::Create(ctx, name);
	}
}

void codegen_env::enter_scope() {
	auto& scope = push_scope();

	// create cleanup block (default unreachable)
	scope.cleanup_block = llvm::BasicBlock::Create(ctx, "scope.clean", &func);
	llvm::IRBuilder<> temp_builder(scope.cleanup_block);
	temp_builder.CreateUnreachable();
}
void codegen_env::enter_loop_scope(llvm::BasicBlock* continue_block, llvm::BasicBlock* break_block) {
	auto& scope = push_scope();

	scope.continue_block = continue_block;
	scope.break_block = break_block;
	scope.is_loop_scope = true;

	// create cleanup block (default unreachable)
	scope.cleanup_block = llvm::BasicBlock::Create(ctx, "scope.clean", &func);
	llvm::IRBuilder<> temp_builder(scope.cleanup_block);
	temp_builder.CreateUnreachable();
}
void codegen_env::leave_scope_inl() {
	if (scope_stack.empty()) {
		return; // should emit an error in the future
	}
	scope s = pop_scope();

	// emit cleanup actions
	for (auto it = s.cleanup_actions.rbegin(); it != s.cleanup_actions.rend(); ++it) {
		emit_cleanup(*it);
	}
}
void codegen_env::register_cleanup(const cleanup_action& action) {
	if (scope_stack.empty()) {
		return; // should emit an error in the future
	}
	scope_stack.back().cleanup_actions.push_back(action);
}

void codegen_env::alloc_return_slot(llvm::Type* ret_type) {
	if (!ret_type->isVoidTy()) {
		ret_slot = create_alloca(ret_type, "ret.slot");
	}
}

std::shared_ptr<variable> codegen_env::alloc_local(llvm::Type* type, const std::string& name) {
	// create alloca
	auto* inst = create_alloca(type, name);

	if (scope_stack.empty()) {
		throw codegen_exception("No active scope to allocate local variable in");
	}

	// create variable
	auto var = std::make_shared<variable>(name, inst);
	scope_stack.back().locals_map[name] = var;

	// return variable pointer
	return var;
}
std::shared_ptr<variable> codegen_env::lookup_local(const std::string& name) {
	for (auto it = scope_stack.rbegin(); it != scope_stack.rend(); ++it) {
		auto found = it->locals_map.find(name);
		if (found != it->locals_map.end()) {
			return found->second;
		}
	}
	return nullptr;
}

std::shared_ptr<lvalue> codegen_env::lvalue_from_expression(std::shared_ptr<expression> expr) {
	auto entity = expr->entity();
	if (entity && entity->kind() == ENTITY_VARIABLE) {
		auto var = lookup_local(entity->name());
		if (!var) {
			throw codegen_exception("Undefined variable: " + entity->name());
		}
		return var;
	}

	// add support for other lvalue expressions here

	// invalid lvalue
	throw codegen_exception("Unsupported lvalue expression (not an lvalue or not implemented)");
}

void codegen_env::emit_life_start(llvm::Value* ptr) {
	// TODO
}
void codegen_env::emit_return(llvm::Value* ret_value) {
	// save return value
	if (ret_value && ret_slot) {
		builder.CreateStore(ret_value, ret_slot);
	}

	// move to cleanup-chain or return block
	if (!scope_stack.empty()) {
		builder.CreateBr(scope_stack.back().cleanup_block);
	}
	else if (ret_block) {
		builder.CreateBr(ret_block);
	}

	// emit cleanup
	build_cleanup_chain();
}
void codegen_env::emit_break() {
	const scope* loop_scope = get_current_loop();
	if (!loop_scope || !loop_scope->break_block) {
		throw codegen_exception("Cannot emit break outside of a loop");
	}
	builder.CreateBr(loop_scope->break_block);
}
void codegen_env::emit_continue() {
	const scope* loop_scope = get_current_loop();
	if (!loop_scope || !loop_scope->continue_block) {
		throw codegen_exception("Cannot emit continue outside of a loop");
	}
	builder.CreateBr(loop_scope->continue_block);
}
void codegen_env::build_cleanup_chain() {
	// build clean up chain based on current scope
	// does NOT regenerate clean up blocks that have
	// already been generated
	for (size_t i = scope_stack.size(); i-- > 0;) {
		scope& s = scope_stack[i];

		if (s.cleanup_emitted) {
			continue;
		}

		// remove unreachable terminator - this cleanup block is used
		s.cleanup_block->getTerminator()->eraseFromParent();
		builder.SetInsertPoint(s.cleanup_block);
		for (auto it = s.cleanup_actions.rbegin(); it != s.cleanup_actions.rend(); ++it) {
			emit_cleanup(*it);
		}
		s.cleanup_emitted = true;

		if (i <= 0) {
			builder.CreateBr(ret_block);
		}
		else {
			builder.CreateBr(scope_stack[i - 1].cleanup_block);
		}
	}
}
void codegen_env::finalize_function() {
	builder.SetInsertPoint(ret_block);

	if (ret_slot) {
		llvm::Value* rv = builder.CreateLoad(ret_slot->getAllocatedType(), ret_slot);
		builder.CreateRet(rv);
	}
	else {
		builder.CreateRetVoid();
	}
}

llvm::AllocaInst* codegen_env::create_alloca(llvm::Type* type, const std::string& name) {
	llvm::IRBuilder<> entry_builder = llvm::IRBuilder<>(&func.getEntryBlock(), func.getEntryBlock().begin());
	return entry_builder.CreateAlloca(type, nullptr, name);
}
void codegen_env::emit_cleanup(const cleanup_action& action) {
	switch (action.kind) {
	case CLEANUP_ACTION_DESTRUCTOR: {
		builder.CreateCall(action.destructor_callee, { action.object_ptr });
		break;
	}
	case CLEANUP_ACTION_LIFETIME_END: {
		// TODO
		break;
	}
	}
}

const scope* codegen_env::get_current_loop() const {
	for (size_t i = scope_stack.size(); i-- > 0;) {
		const scope& s = scope_stack[i];
		if (s.is_loop_scope) {
			return &s;
		}
	}
	return nullptr;
}
