#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include <llvm/IR/BasicBlock.h>

#include <codegen/cleanup/cleanup_action.h>
#include <codegen/memory/variable.h>

struct scope {
	llvm::BasicBlock* cleanup_block = nullptr;
	llvm::BasicBlock* continue_block = nullptr;
	llvm::BasicBlock* break_block = nullptr;
	std::vector<cleanup_action> cleanup_actions;
	std::unordered_map<std::string, std::shared_ptr<variable>> locals_map;

	bool cleanup_emitted = false;
	bool is_loop_scope = false;
};