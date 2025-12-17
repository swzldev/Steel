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

llvm::Function* llvm_function_builder::build(const mir_function& fn_mir, llvm::Module* module) {
	auto linkage = llvm::Function::ExternalLinkage;
	auto fn_type = get_llvm_fn_type(fn_mir);

	std::string name = mangler.mangle_function(fn_mir);
	llvm::Function* fn = llvm::Function::Create(
		/* Function Type */ fn_type,
		/* Linkage */ linkage,
		/* Name */ name,
		/* Module */ module
	);

	// name the function arguments
	/*auto arg_iter = fn->arg_begin();
	if (func_ast->is_method) {
		// name this pointer (if applicable)
		arg_iter->setName(codegen_constants::THIS_PARAM_NAME);
		++arg_iter;
	}
	for (const auto& param : fn_mir.param_types) {
		arg_iter->setName(param->identifier);
		++arg_iter;
	}*/ // temporarily disabled as mir_function does not store parameter names

	return fn;
}
llvm::FunctionType* llvm_function_builder::get_llvm_fn_type(const mir_function& fn_mir) {
	auto return_type = type_converter.convert(fn_mir.return_type);

	llvm::FunctionType* fn_type = nullptr;
	if (fn_mir.param_types.empty()) {
		fn_type = llvm::FunctionType::get(
			/* Return Type */ return_type,
			/* isVarArg */ false
		);
	}
	else {
		std::vector<llvm::Type*> llvm_param_types;
		for (const auto& param : fn_mir.param_types) {
			llvm_param_types.push_back(type_converter.convert(mir_type{ param.ty }));
		}
		fn_type = llvm::FunctionType::get(
			/* Return Type */ return_type,
			/* Param Types */ llvm_param_types,
			/* isVarArg */ false
		);
		// in the future, we can add support for variadic functions here
	}

	return fn_type;
}

name_mangler llvm_function_builder::mangler;
