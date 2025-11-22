#include "cleanup_action.h"

#include <llvm/IR/Value.h>

cleanup_action cleanup_action::create_lifetime_end(llvm::Value* obj) {
	cleanup_action action(CLEANUP_ACTION_LIFETIME_END);
	action.object_ptr = obj;
	return action;
}
cleanup_action cleanup_action::create_destructor(llvm::Value* obj, llvm::FunctionCallee dtor) {
	cleanup_action action(CLEANUP_ACTION_DESTRUCTOR);
	action.object_ptr = obj;
	action.destructor_callee = dtor;
	return action;
}
