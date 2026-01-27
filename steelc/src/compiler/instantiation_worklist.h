#pragma once

#include <memory>
#include <queue>
#include <unordered_set>

#include <ast/ast_fwd.h>

class instantiation_worklist {
public:
	inline void enqueue(std::shared_ptr<function_declaration> func_decl) {
		auto* raw = func_decl.get();
		if (seen.insert(raw).second) {
			q.push(func_decl);
		}
	}
	inline bool dequeue(std::shared_ptr<function_declaration>& out_func_decl) {
		if (q.empty()) {
			return false;
		}
		out_func_decl = q.front();
		q.pop();
		return true;
	}

private:
	std::queue<std::shared_ptr<function_declaration>> q;
	std::unordered_set<function_declaration*> seen;
};