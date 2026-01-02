#include "init_checker.h"

#include <unordered_set>

#include <ast/ast.h>
#include <representations/entities/entities_fwd.h>
#include <representations/entities/entity.h>
#include <representations/entities/variable_entity.h>
#include <representations/types/types_fwd.h>
#include <representations/types/container_types.h>

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
	}
	else if (!default_initialized(var->type)) {
		return;
	}

	initialized.insert(var);
	var->initialized = true;
}
void init_checker::visit(std::shared_ptr<assignment_expression> expr) {
	// dont accept left - it might be uninitialized, however
	// since were not using it we dont want to throw an error
	expr->right->accept(*this);

	auto entity = expr->left->entity();
	if (entity && entity->kind() == ENTITY_VARIABLE) {
		auto decl = entity->as_variable()->declaration;
		initialized.insert(decl);
		decl->initialized = true;
	}
}
void init_checker::visit(std::shared_ptr<identifier_expression> expr) {
	// if its not a variable we can safely ignore this
	if (expr->entity()->kind() != ENTITY_VARIABLE) {
		return;
	}

	if (initialized.find(expr->entity()->as_variable()->declaration) == initialized.end()) {
		ERROR(ERR_UNINITIALIZED_VARIABLE, expr->span, expr->identifier.c_str());
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

bool init_checker::default_initialized(type_ptr type) {
	// for now we can allow fixed-size arrays but more will be allowed later
	if (auto arr = type->as_array()) {
		return arr->size_expression != nullptr;
	}
}
