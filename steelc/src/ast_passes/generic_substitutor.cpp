#include "generic_substitutor.h"

#include <ast/ast.h>
#include <representations/types/types_fwd.h>
#include <representations/types/data_type.h>
#include <representations/types/custom_type.h>
#include <representations/types/container_types.h>
#include <representations/types/core.h>
#include <representations/types/type_utils.h>

void generic_substitutor::visit(std::shared_ptr<function_declaration> func) {
	for (auto& param : func->parameters) {
		param->accept(*this);
	}
	if (func->return_type) {
		try_substitute(func->return_type);
	}
	if (func->body) {
		func->body->accept(*this);
	}
}
void generic_substitutor::visit(std::shared_ptr<variable_declaration> var) {
	try_substitute(var->type);
	if (var->has_initializer()) {
		var->initializer->accept(*this);
	}
}
void generic_substitutor::visit(std::shared_ptr<type_declaration> decl) {
	for (const auto& ctor : decl->constructors) {
		ctor->accept(*this);
	}
	for (const auto& field : decl->fields) {
		field->accept(*this);
	}
	for (const auto& method : decl->methods) {
		method->accept(*this);
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}
}
void generic_substitutor::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<assignment_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<address_of_expression> expr) {
	expr->value->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<deref_expression> expr) {
	expr->value->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<unary_expression> expr) {
	expr->operand->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<index_expression> expr) {
	expr->base->accept(*this);
	expr->indexer->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<cast_expression> expr) {
	expr->expr->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<member_expression> expr) {
	expr->object->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<initializer_list> init) {
	for (const auto& value : init->values) {
		value->accept(*this);
	}
}
void generic_substitutor::visit(std::shared_ptr<function_call> func_call) {
	for (const auto& arg : func_call->args) {
		arg->accept(*this);
	}
}
void generic_substitutor::visit(std::shared_ptr<if_statement> if_stmt) {
	if_stmt->condition->accept(*this);
	if_stmt->then_block->accept(*this);
	if (if_stmt->else_node) {
		if_stmt->else_node->accept(*this);
	}
}
void generic_substitutor::visit(std::shared_ptr<inline_if> inline_if) {
	inline_if->condition->accept(*this);
	inline_if->statement->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<for_loop> for_loop) {
	if (for_loop->initializer) {
		for_loop->initializer->accept(*this);
	}
	if (for_loop->condition) {
		for_loop->condition->accept(*this);
	}
	if (for_loop->increment) {
		for_loop->increment->accept(*this);
	}
	for_loop->body->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<while_loop> while_loop) {
	while_loop->condition->accept(*this);
	while_loop->body->accept(*this);
}
void generic_substitutor::visit(std::shared_ptr<return_statement> ret) {
	if (ret->is_conditional()) {
		ret->condition->accept(*this);
	}
	if (ret->value) {
		ret->value->accept(*this);
	}
}

type_ptr generic_substitutor::get_substitution(size_t parameter_index) const {
	return parameter_index < substitution_list.size() ? substitution_list[parameter_index] : data_type::UNKNOWN;
}
void generic_substitutor::try_substitute(type_ptr& type_ref) const {
	if (auto arr = type_ref->as_array()) {
		try_substitute(arr->base_type);
		return;
	}
	else if (auto ptr = type_ref->as_pointer()) {
		try_substitute(ptr->base_type);
		return;
	}

	if (!type_ref->is_generic()) return;

	auto generic = type_ref->as_generic();
	type_ref = get_substitution(generic->generic_param_index);
}
