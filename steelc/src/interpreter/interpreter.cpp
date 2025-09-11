#include "interpreter.h"

#include <iostream>

#include "interpreter_visitor.h"

void interpreter::begin_execution() {
	interpreter_visitor visitor;
	
	// get the main function
	auto& main_func = module_manager.main_declaration;
	if (main_func == nullptr) {
		std::cerr << "Interpreter error: No 'Main' function found in the program." << std::endl;
		return;
	}

	visitor.begin(main_func);
}