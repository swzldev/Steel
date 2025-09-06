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

void interpreter::add_builtin_function(const std::string& identifier, type_ptr return_type, const std::vector<std::shared_ptr<variable_declaration>>& parameters) {
	function_declaration decl = { return_type, identifier, parameters, {} };
	module_manager.builtin_functions.push_back(std::make_shared<function_declaration>(decl));
}