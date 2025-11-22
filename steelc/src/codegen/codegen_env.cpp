#include "codegen_env.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IntrinsicEnums.inc>
#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Intrinsics.h>

void codegen_env::enter_scope() {
	scope_stack.push_back(scope{});
	auto& scope = scope_stack.back();
	// create cleanup block
	scope.cleanup_block = llvm::BasicBlock::Create(ctx, "scope.clean", &func);
}
void codegen_env::leave_scope_inl() {
	if (scope_stack.empty()) {
		return; // should emit an error in the future
	}
	scope s = scope_stack.back();
	scope_stack.pop_back();

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
		ret_slot = alloc_local(ret_type, "ret.slot");
	}
}

llvm::AllocaInst* codegen_env::alloc_local(llvm::Type* type, const std::string& name) {
	llvm::IRBuilder<> entry_builder(&func.getEntryBlock(), func.getEntryBlock().begin());
	auto* inst = entry_builder.CreateAlloca(type, nullptr, name);
	register_local(name, inst);
	return inst;
}
void codegen_env::register_local(const std::string& name, llvm::AllocaInst* local) {
	if (scope_stack.empty()) {
		return; // should emit an error in the future
	}
	scope_stack.back().locals_map[name] = local;
}
llvm::AllocaInst* codegen_env::lookup_local(const std::string& name) {
	for (auto it = scope_stack.rbegin(); it != scope_stack.rend(); ++it) {
		auto found = it->locals_map.find(name);
		if (found != it->locals_map.end()) {
			return found->second;
		}
	}
	return nullptr;
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
void codegen_env::build_cleanup_chain() {
	// build clean up chain based on current scope
	// does NOT regenerate clean up blocks that have
	// already been generated
	for (size_t i = scope_stack.size(); i-- > 0;) {
		scope& s = scope_stack[i];

		if (s.cleanup_emitted) {
			continue;
		}

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