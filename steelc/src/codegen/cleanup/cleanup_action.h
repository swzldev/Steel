#pragma once

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>

enum cleanup_action_kind {
	CLEANUP_ACTION_LIFETIME_END,
	CLEANUP_ACTION_DESTRUCTOR,
};

class cleanup_action {
public:
	cleanup_action_kind kind;

	llvm::FunctionCallee destructor_callee;		// only for CLEANUP_ACTION_DESTRUCTOR
	llvm::Value* object_ptr;					// object being cleaned up

	static cleanup_action create_lifetime_end(llvm::Value* obj);
	static cleanup_action create_destructor(llvm::Value* obj, llvm::FunctionCallee dtor);

private:
	cleanup_action(cleanup_action_kind kind)
		: kind(kind), destructor_callee(nullptr), object_ptr(nullptr) {
	}
};