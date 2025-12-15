#include "codegen_visitor.h"

#include <string>
#include <memory>
#include <vector>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/DerivedTypes.h>

#include <codegen/error/codegen_exception.h>
#include <codegen/memory/variable.h>
#include <codegen/codegen_env.h>
#include <ast/ast.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>

void codegen_visitor::visit(std::shared_ptr<function_declaration> func) { 
	if (func->is_generic && !func->is_generic_instance) {
		// dont generate code for generic templates
		return;
	}

	llvm::Function* fn = function_builder.build(func);

	// function entry block
	llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(context, "entry", fn);
	builder.SetInsertPoint(entry_block);

	env = std::make_unique<codegen_env>(module, *fn, builder);
	cg_assert(env != nullptr, "Failed to create codegen environment for function " + func->identifier);
	env->builder.SetInsertPoint(entry_block);

	// allocate return slot if needed
	if (!func->return_type->is_void()) {
		env->alloc_return_slot(type_converter.convert(func->return_type));
	}

	// create initial scope
	env->enter_scope();

	// allocate params
	int i = 0;
	for (auto& arg : fn->args()) {
		auto param = env->alloc_local(arg.getType(), func->parameters[i]->identifier);
		env->builder.CreateStore(&arg, param->get_slot());
		i++;
	}

	func->body->accept(*this);

	if (func->implicitly_returns) {
		env->emit_return();
	}

	// cleanup
	env->leave_scope_inl();

	env->finalize_function();
}
void codegen_visitor::visit(std::shared_ptr<variable_declaration> var) {
	if (var->is_field) {
		// dont allocate fields here
		// theyre only allocated as part of object instantiation
		return;
	}

	llvm::Type* var_type = type_converter.convert(var->type);
	auto var_inst = env->alloc_local(var_type, var->identifier);

	cg_assert(var_inst != nullptr, "Failed to allocate local variable " + var->identifier);

	//env->emit_life_start(var);
	//env->register_cleanup(cleanup_action::create_lifetime_end(var));

	// initialize variable if there is an initializer
	if (var->initializer) {
		llvm::Value* init_value = accept(var->initializer);
		cg_assert(init_value != nullptr, "Failed to generate initializer for variable " + var->identifier);

		env->builder.CreateStore(init_value, var_inst->get_slot());
	}

	// TODO: add support for destructors
}
void codegen_visitor::visit(std::shared_ptr<binary_expression> expr) {
	llvm::Value* lhs = accept(expr->left);
	llvm::Value* rhs = accept(expr->right);

	cg_assert(lhs != nullptr, "Failed to generate left operand of binary expression");
	cg_assert(rhs != nullptr, "Failed to generate right operand of binary expression");

	result = expression_builder.build_binary_expr(lhs, rhs, expr->oparator);
}
void codegen_visitor::visit(std::shared_ptr<assignment_expression> expr) {
	auto rhs = accept(expr->right);
	cg_assert(rhs != nullptr, "Failed to generate right-hand side of assignment expression");

	auto lval = env->lvalue_from_expression(expr->left);
	cg_assert(lval->valid(), "Left-hand side of assignment is not a valid lvalue");

	lval->store(env->builder, rhs);
}
void codegen_visitor::visit(std::shared_ptr<member_expression> expr) {
	if (expr->is_static_access()) {
		// static accesses are purely semantic
		// shouldnt compile to anything
		return;
	}
	else if (expr->is_instance_access()) {
		auto object = accept(expr->object);
		cg_assert(object != nullptr, "Failed to generate object of member expression");

		auto object_entity = expr->object->entity();
		cg_assert(object_entity != entity::UNRESOLVED, "Member expression object entity is unresolved");

		auto object_type = type_converter.convert(object_entity->as_variable()->var_type());

		auto entity = expr->entity();
		cg_assert(entity != entity::UNRESOLVED, "Member expression entity is unresolved");

		switch (entity->kind()) {
		case ENTITY_VARIABLE: {
			auto var_entity = entity->as_variable();
			result = expression_builder.build_field_access(
				/* this should be safe, as sema should catch member access on non-composites */
				object_type,
				object,
				var_entity->declaration->field_index
			);
			break;
		}

		default:
			throw codegen_exception("Unsupported entity in member expression");
		}

		return;
	}
	
	throw codegen_exception("Unknown member expression access type");
}
void codegen_visitor::visit(std::shared_ptr<address_of_expression> expr) {
	auto lval = env->lvalue_from_expression(expr->value);
	cg_assert(lval->valid(), "Operand of address-of expression is not a valid lvalue");
	result = lval->address();
}
void codegen_visitor::visit(std::shared_ptr<deref_expression> expr) {
	auto lval = env->lvalue_from_expression(expr->value);
	cg_assert(lval->valid(), "Operand of deref expression is not a valid lvalue");
	result = lval->load(env->builder);
}
void codegen_visitor::visit(std::shared_ptr<index_expression> expr) {
	auto base = accept(expr->base);
	cg_assert(base != nullptr, "Failed to generate base of index expression");
	auto index = accept(expr->indexer);
	cg_assert(index != nullptr, "Failed to generate index of index expression");

	result = expression_builder.build_index_expr(base->getType(), base, index);
}
void codegen_visitor::visit(std::shared_ptr<unary_expression> expr) {
	llvm::Value* operand = nullptr;
	if (op_requires_lvalue(expr->oparator)) {
		auto lval = env->lvalue_from_expression(expr->operand);
		cg_assert(lval->valid(), "Left-hand side of assignment is not a valid lvalue");
		operand = lval->address();
	}
	else {
		operand = accept(expr->operand);
	}
	cg_assert(operand != nullptr, "Failed to generate operand of unary expression");

	llvm::Type* expr_type = type_converter.convert(expr->type());

	result = expression_builder.build_unary_expr(expr_type, operand, expr->oparator);
}
void codegen_visitor::visit(std::shared_ptr<identifier_expression> id) {
	if (id->entity()->kind() == ENTITY_VARIABLE) {
		auto var = env->lookup_local(id->identifier);
		cg_assert(var != nullptr, "Undefined variable: " + id->identifier);

		result = var->load(env->builder);
	}
}
void codegen_visitor::visit(std::shared_ptr<initializer_list> init) {
	if (init->is_array_initializer) {
		return; // not implemented yet
	}

	std::vector<llvm::Value*> element_values;
	for (auto& val : init->values) {
		auto elem_value = accept(val);
		cg_assert(elem_value != nullptr, "Failed to generate value for initializer list element");
		element_values.push_back(elem_value);
	}

	llvm::Type* struct_type = type_converter.convert(init->type());
	result = expression_builder.build_struct_init(struct_type, element_values);
}
void codegen_visitor::visit(std::shared_ptr<function_call> func_call) {
	std::vector<llvm::Value*> arg_values;

	for (auto& arg : func_call->args) {
		arg_values.push_back(accept(arg));
		cg_assert(result != nullptr, "Failed to generate argument for function call: " + func_call->identifier);
	}

	if (func_call->is_method()) {

	}
	else if (func_call->is_constructor) {

	}
	else {
		if (func_call->identifier == "printf") {
			auto* printf_type = llvm::FunctionType::get(
				llvm::Type::getInt32Ty(context),
				llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context)),
				true
			);

			auto* printf_fn = llvm::cast<llvm::Function>(module.getOrInsertFunction("printf", printf_type).getCallee());

			result = env->builder.CreateCall(printf_fn, arg_values);
		}

		else {
			if (func_call->declaration) {
				if (func_call->declaration->is_generic && !func_call->declaration->is_generic_instance) {
					throw codegen_exception("Cannot call generic function template without instantiation: " + func_call->identifier);
				}
				else if (func_call->declaration->is_generic) {
					// accept here, as generic instantiations do
					// not get added to the program declarations
					func_call->declaration->accept(*this);
				}

				// get the full name
				std::string mangled_name = mangler.mangle_function(func_call->declaration);

				// lookup in current module
				llvm::Function* callee_fn = module.getFunction(func_call->identifier);
				if (callee_fn == nullptr) {
					// assume external
					auto callee_type = function_builder.get_llvm_fn_type(func_call->declaration);
					callee_fn = llvm::cast<llvm::Function>(module.getOrInsertFunction(mangled_name, callee_type).getCallee());
					callee_fn->setLinkage(llvm::Function::ExternalLinkage);
				}
				result = env->builder.CreateCall(callee_fn, arg_values);
			}
			else {
				throw codegen_exception("Function call to undefined function: " + func_call->identifier);
			}
		}
	}
}
void codegen_visitor::visit(std::shared_ptr<literal> literal) {
	result = generate_literal(literal);
}
void codegen_visitor::visit(std::shared_ptr<code_block> block) {
	if (!block->is_body) {
		env->enter_scope();
	}
	for (const auto& stmt : block->body) {
		stmt->accept(*this);
	}
	if (!block->is_body) {
		env->leave_scope_inl();
	}
}
void codegen_visitor::visit(std::shared_ptr<if_statement> if_stmt) {
	auto condition = accept(if_stmt->condition);
	cg_assert(condition != nullptr, "Failed to generate condition for if statement");

	auto* then_block = llvm::BasicBlock::Create(context, "if.then", &env->func);
	auto* else_block = llvm::BasicBlock::Create(context, "if.else");
	auto* merge_block = llvm::BasicBlock::Create(context, "if.merge");
	
	if (if_stmt->else_node) {
		builder.CreateCondBr(condition, then_block, else_block);
	}
	else {
		builder.CreateCondBr(condition, then_block, merge_block);
	}

	// generate then block
	builder.SetInsertPoint(then_block);

	env->enter_scope();
	if_stmt->then_block->accept(*this);
	env->leave_scope_inl();

	if (!builder.GetInsertBlock()->getTerminator()) {
		builder.CreateBr(merge_block);
	}

	// generate else block
	if (if_stmt->else_node) {
		env->func.insert(env->func.end(), else_block);
		builder.SetInsertPoint(else_block);

		env->enter_scope();
		if_stmt->else_node->accept(*this);
		env->leave_scope_inl();

		if (!builder.GetInsertBlock()->getTerminator()) {
			builder.CreateBr(merge_block);
		}
	}

	// continue at merge block
	env->func.insert(env->func.end(), merge_block);
	builder.SetInsertPoint(merge_block);
}
void codegen_visitor::visit(std::shared_ptr<for_loop> for_loop) {
	auto init_block = env->make_block("for.init");
	auto cond_block = env->make_block("for.cond");
	auto body_block = env->make_block("for.body");
	auto incr_block = env->make_block("for.incr");
	auto merge_block = env->make_block("for.merge");

	// initialzer
	env->builder.CreateBr(init_block);
	env->builder.SetInsertPoint(init_block);
	if (for_loop->initializer) {
		for_loop->initializer->accept(*this);
	}
	env->builder.CreateBr(cond_block);

	// condition
	env->builder.SetInsertPoint(cond_block);
	if (for_loop->condition) {
		auto cond_value = accept(for_loop->condition);
		cg_assert(cond_value != nullptr, "Failed to generate condition for for loop");
		env->builder.CreateCondBr(cond_value, body_block, merge_block);
	}
	else {
		// no condition means always true
		env->builder.CreateBr(body_block);
	}

	// body
	env->builder.SetInsertPoint(body_block);
	env->enter_loop_scope(cond_block, merge_block);
	for_loop->body->accept(*this);
	env->leave_scope_inl();
	if (!env->builder.GetInsertBlock()->getTerminator()) {
		env->builder.CreateBr(incr_block);
	}

	// increment
	env->builder.SetInsertPoint(incr_block);
	if (for_loop->increment) {
		for_loop->increment->accept(*this);
	}
	env->builder.CreateBr(cond_block);

	// merge
	env->builder.SetInsertPoint(merge_block);
}
void codegen_visitor::visit(std::shared_ptr<while_loop> while_loop) {
	auto cond_block = env->make_block("while.cond");
	auto body_block = env->make_block("while.body");
	auto merge_block = env->make_block("while.merge");

	// jump to condition
	env->builder.CreateBr(cond_block);

	// condition
	env->builder.SetInsertPoint(cond_block);
	auto cond_value = accept(while_loop->condition);
	cg_assert(cond_value != nullptr, "Failed to generate condition for while loop");
	env->builder.CreateCondBr(cond_value, body_block, merge_block);

	// body
	env->builder.SetInsertPoint(body_block);
	env->enter_loop_scope(cond_block, merge_block);
	while_loop->body->accept(*this);
	env->leave_scope_inl();
	if (!env->builder.GetInsertBlock()->getTerminator()) {
		env->builder.CreateBr(cond_block);
	}

	// merge
	env->builder.SetInsertPoint(merge_block);
}
void codegen_visitor::visit(std::shared_ptr<return_statement> ret_stmt) {
	llvm::Value* ret_value = nullptr;
	if (ret_stmt->value) {
		ret_stmt->value->accept(*this);
		ret_value = result;
	}

	env->emit_return(ret_value);
}
void codegen_visitor::visit(std::shared_ptr<break_statement> brk_stmt) {
	env->emit_break();
}

llvm::Value* codegen_visitor::generate_literal(std::shared_ptr<literal> lit) {
	switch (lit->primitive) {
	case DT_I16: {
		short value = std::stoi(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(16, value, true));
	}
	case DT_I32: {
		int value = std::stoi(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(32, value, true));
	}
	case DT_I64: {
		long long value = std::stoll(lit->value);
		return llvm::ConstantInt::get(context, llvm::APInt(64, value, true));
	}
	case DT_BOOL: {
		return llvm::ConstantInt::get(context, llvm::APInt(1, lit->value == "true" ? 1 : 0, false));
	}
	case DT_FLOAT: {
		float fvalue = std::stof(lit->value);
		return llvm::ConstantFP::get(context, llvm::APFloat(fvalue));
	}
	case DT_DOUBLE: {
		double dvalue = std::stod(lit->value);
		return llvm::ConstantFP::get(context, llvm::APFloat(dvalue));
	}
	case DT_STRING: {
		return builder.CreateGlobalStringPtr(lit->value);
	}
	case DT_CHAR: {
		char cvalue = lit->value[0];
		return llvm::ConstantInt::get(context, llvm::APInt(8, cvalue, false));
	}
	case DT_BYTE: {
		unsigned char bvalue = static_cast<unsigned char>(std::stoi(lit->value));
		return llvm::ConstantInt::get(context, llvm::APInt(8, bvalue, false));
	}
	}
}

bool codegen_visitor::op_requires_lvalue(token_type op) {
	if (op == TT_INCREMENT ||
		op == TT_DECREMENT) {
		return true;
	}
	return false;
}
