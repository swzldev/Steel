#include "init_checker.h"

#include "../ast/ast.h"

void init_checker::visit(std::shared_ptr<type_declaration> decl) {
	// mark all fields as initialized
	// if they arnt, that will be picked up elsewhere
	for (const auto& field : decl->fields) {
		initialized.insert(field);
		field->initialized = true;
	}
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
void init_checker::visit(std::shared_ptr<function_declaration> func) {
	// mark all parameters as initialized
	for (const auto& param : func->parameters) {
		initialized.insert(param);
		param->initialized = true;
	}
	if (func->body) {
		func->body->accept(*this);
	}
}
void init_checker::visit(std::shared_ptr<variable_declaration> var) {
	if (var->has_initializer()) {
		var->initializer->accept(*this);
		initialized.insert(var);
		var->initialized = true;
	}
}
void init_checker::visit(std::shared_ptr<assignment_expression> expr) {
	expr->right->accept(*this);

	auto id = std::dynamic_pointer_cast<identifier_expression>(expr->left);
	if (id && id->declaration) {
		initialized.insert(id->declaration);
		id->declaration->initialized = true;
	}
}
void init_checker::visit(std::shared_ptr<identifier_expression> expr) {
	if (!expr->declaration) {
		// if it hasnt been resolved, it means its probably invalid, so we can ignore it
		return;
	}
	// assuming identifier is variable access
	if (initialized.find(expr->declaration) == initialized.end()) {
		ERROR(ERR_UNINITIALIZED_VARIABLE, expr->position, expr->identifier.c_str());
		return;
	}
}
void init_checker::visit(std::shared_ptr<code_block> block) {
	auto before = initialized;
	for (auto& stmt : block->body) {
		stmt->accept(*this);
	}
	initialized = before;
}
void init_checker::visit(std::shared_ptr<if_statement> if_stmt) {
	if_stmt->condition->accept(*this);

	auto before = initialized;
	auto then_set = initialized;
	auto then_block = std::dynamic_pointer_cast<code_block>(if_stmt->then_block);
	traverse_block(then_block, false);
	then_set = initialized;

	auto else_set = before;
	if (if_stmt->else_node) {
		initialized = before;
		if (auto else_node = std::dynamic_pointer_cast<code_block>(if_stmt->else_node)) {
			traverse_block(else_node, false);
		}
		else {
			// usually an else-if statement, so we can accept as normal
			if_stmt->else_node->accept(*this);
		}
		else_set = initialized;
	}

	// keep only variables that were initialized in both branches
	std::unordered_set<std::shared_ptr<variable_declaration>> intersection;
	for (auto var : then_set) {
		if (else_set.count(var)) intersection.insert(var);
	}
	initialized = intersection;
}
void init_checker::visit(std::shared_ptr<for_loop> for_loop) {
	// may never run, assume initializations could never occur
	for_loop->initializer->accept(*this);
	for_loop->condition->accept(*this);

	auto before = initialized;
	auto block = std::dynamic_pointer_cast<code_block>(for_loop->body);
	traverse_block(block, false);

	initialized = before;
}
void init_checker::visit(std::shared_ptr<while_loop> while_loop) {
	// may never run, assume initializations could never occur
	while_loop->condition->accept(*this);

	auto before = initialized;
	auto block = std::dynamic_pointer_cast<code_block>(while_loop->body);
	traverse_block(block, false);

	initialized = before;
}

void init_checker::traverse_block(std::shared_ptr<code_block>& block, bool restore) {
	auto before = initialized;
	for (auto& stmt : block->body) {
		stmt->accept(*this);
	}
	if (restore) {
		initialized = before;
	}
}