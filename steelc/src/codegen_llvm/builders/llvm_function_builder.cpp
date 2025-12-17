#include "llvm_function_builder.h"

#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

#include <codegen/naming/name_mangler.h>
#include <codegen/constants/codegen_constants.h>
#include <codegen/error/codegen_exception.h>
#include <ast/declarations/function_declaration.h>
#include <ast/declarations/type_declaration.h>

llvm::Function* llvm_function_builder::build(std::shared_ptr<function_declaration> func_ast) {
	if (func_ast->is_generic && !func_ast->is_generic_instance) {
		// cannot build generic function templates
		throw codegen_exception("Attempted to build generic function: " + func_ast->identifier);
	}

	auto linkage = llvm::Function::ExternalLinkage;
	auto fn_type = get_llvm_fn_type(func_ast);

	std::string name = mangler.mangle_function(func_ast);
	llvm::Function* fn = llvm::Function::Create(
		/* Function Type */ fn_type,
		/* Linkage */ linkage,
		/* Name */ name,
		/* Module */ module
	);

	// name the function arguments
	auto arg_iter = fn->arg_begin();
	if (func_ast->is_method) {
		// name this pointer (if applicable)
		arg_iter->setName(codegen_constants::THIS_PARAM_NAME);
		++arg_iter;
	}
	for (const auto& param : func_ast->parameters) {
		arg_iter->setName(param->identifier);
		++arg_iter;
	}

	return fn;
}
llvm::FunctionType* llvm_function_builder::get_llvm_fn_type(std::shared_ptr<function_declaration> func_ast) {
	auto return_type = type_converter.convert(func_ast->return_type);

	llvm::FunctionType* fn_type = nullptr;
	if (func_ast->parameters.empty()) {
		fn_type = llvm::FunctionType::get(
			/* Return Type */ return_type,
			/* isVarArg */ false
		);
	}
	else {
		std::vector<llvm::Type*> param_types;
		if (func_ast->is_method) {
			// add 'this' pointer
			param_types.push_back(type_converter.convert(func_ast->parent_type->type()));
		}
		for (const auto& param : func_ast->parameters) {
			param_types.push_back(type_converter.convert(param->type));
		}
		fn_type = llvm::FunctionType::get(
			/* Return Type */ return_type,
			/* Param Types */ param_types,
			/* isVarArg */ false
		);
		// in the future, we can add support for variadic functions here
	}

	return fn_type;
}

name_mangler llvm_function_builder::mangler;
