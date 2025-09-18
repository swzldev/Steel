#include "ast_visitor.h"

#include "ast.h"

void ast_visitor::visit(std::shared_ptr<compilation_unit> program) {
	for (const auto& decl : program->declarations) {
		decl->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<function_declaration> func) {
	if (func->body) {
		func->body->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<variable_declaration> var) {

}
void ast_visitor::visit(std::shared_ptr<type_declaration> decl) {
	for (const auto& constructor : decl->constructors) {
		constructor->accept(*this);
	}
	for (const auto& member : decl->fields) {
		member->accept(*this);
	}
	for (const auto& method : decl->methods) {
		method->accept(*this);
	}
	for (const auto& op : decl->operators) {
		op->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<module_declaration> mod) {
	for (const auto& decl : mod->declarations) {
		decl->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<conversion_declaration> conv) {

}
void ast_visitor::visit(std::shared_ptr<operator_declaration> op) {

}
void ast_visitor::visit(std::shared_ptr<expression_statement> expr) {
	expr->expr->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<assignment_expression> expr) {
	expr->left->accept(*this);
	expr->right->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<member_expression> expr) {
	expr->object->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<address_of_expression> expr) {
	expr->value->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<unary_expression> expr) {
	expr->operand->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<index_expression> expr) {
	expr->base->accept(*this);
	expr->indexer->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<identifier_expression> expr) {

}
void ast_visitor::visit(std::shared_ptr<this_expression> expr) {

}
void ast_visitor::visit(std::shared_ptr<cast_expression> expr) {
	expr->expression->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<initializer_list> init) {
	for (const auto& value : init->values) {
		value->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<function_call> func_call) {
	for (auto& arg : func_call->args) {
		arg->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<literal> literal) {

}
void ast_visitor::visit(std::shared_ptr<import_statement> import_stmt) {
	
}
void ast_visitor::visit(std::shared_ptr<block_statement> block) {
	for (const auto& stmt : block->body) {
		stmt->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<if_statement> if_stmt) {
	if_stmt->condition->accept(*this);
	if_stmt->then_block->accept(*this);
	if (if_stmt->else_block) {
		if_stmt->else_block->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<inline_if> inline_if) {
	inline_if->condition->accept(*this);
	inline_if->statement->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<for_loop> for_loop) {
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
void ast_visitor::visit(std::shared_ptr<while_loop> while_loop) {
	while_loop->condition->accept(*this);
	while_loop->body->accept(*this);
}
void ast_visitor::visit(std::shared_ptr<return_statement> ret_stmt) {
	if (ret_stmt->value) {
		ret_stmt->value->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<return_if> ret_stmt) {
	ret_stmt->condition->accept(*this);
	if (ret_stmt->value) {
		ret_stmt->value->accept(*this);
	}
}
void ast_visitor::visit(std::shared_ptr<generic_parameter> param) {

}