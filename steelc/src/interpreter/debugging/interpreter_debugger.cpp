#include "interpreter_debugger.h"
#include "interpreter_debugger.h"

#include <Windows.h>

void interpreter_debugger::breakpoint(ast_ptr node) {
	is_debugging = true;

	MessageBeep(MB_ICONHAND);


	current_node = node;
}

void interpreter_debugger::begin_console() {
	console.open();
	console.set_context(
		
	);
}
