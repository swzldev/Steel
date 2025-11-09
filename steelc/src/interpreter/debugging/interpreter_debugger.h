#pragma once

#include "debug_console.h"
#include "../../parser/ast/ast.h"

class interpreter_debugger {
public:
	interpreter_debugger() = default;

	void breakpoint(ast_ptr node);

private:
	bool is_debugging = false;
	ast_ptr current_node;
	debug_console console;

	void begin_console();
};