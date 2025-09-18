#include "interpreter_visitor.h"

#include <iostream>
#include <memory>
#include <string>

#include "../parser/ast/ast.h"
#include "../parser/types/custom_types.h"

void interpreter_visitor::begin(std::shared_ptr<function_declaration> entry_point) {
	std::cout << "Starting execution...\n" << std::endl;

	enter_function(entry_point, {});

	// execution finished
	std::cout << "\nExecution finished successfully.\nExit code: " << expression_result->as_int() << std::endl;
}

void interpreter_visitor::visit(std::shared_ptr<function_declaration> func) {
	func->body->accept(*this);
}
void interpreter_visitor::visit(std::shared_ptr<variable_declaration> var) {
	if (var->initializer) {
		// handle initializer list for custom types
		if (auto init_list = std::dynamic_pointer_cast<initializer_list>(var->initializer)) {
			// check if type is custom
			auto custom = std::dynamic_pointer_cast<custom_type>(var->type);
			if (custom && custom->declaration) {
				auto obj = std::make_shared<runtime_value>(*var->type, "");
				// assign initializer values to fields
				for (size_t i = 0; i < init_list->values.size(); i++) {
					auto& member_decl = custom->declaration->fields[i];
					init_list->values[i]->accept(*this);
					obj->set_member(member_decl->identifier, std::make_shared<runtime_value>(*expression_result));
				}
				add_var(var->identifier, obj);
				return;
			}
		}
		var->initializer->accept(*this);
		add_var(var->identifier, std::make_shared<runtime_value>(*expression_result));
	}
	else {
		// handle default initialization for custom types
		auto custom = std::dynamic_pointer_cast<custom_type>(var->type);
		if (custom && custom->declaration) {
			auto obj = std::make_shared<runtime_value>(*var->type, "");
			for (auto& member : custom->declaration->fields) {
				obj->set_member(member->identifier, std::make_shared<runtime_value>(*member->type, ""));
			}
			add_var(var->identifier, obj);
			return;
		}
		add_var(var->identifier, std::make_shared<runtime_value>(*var->type, ""));
	}
}
void interpreter_visitor::visit(std::shared_ptr<type_declaration> decl) {

}
void interpreter_visitor::visit(std::shared_ptr<expression_statement> expr) {
	expr->expr->accept(*this);
}
void interpreter_visitor::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	auto left = std::make_shared<runtime_value>(*expression_result);
	expr->right->accept(*this);
	auto right = std::make_shared<runtime_value>(*expression_result);
	switch (expr->oparator) {
	default:
		switch (expr->oparator) {
		case TT_EQUAL:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), (*left == *right ? "true" : "false"));
			break;
		case TT_NOT_EQUAL:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), (*left != *right ? "true" : "false"));
			break;
		case TT_LESS:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), (*left < *right ? "true" : "false"));
			break;
		case TT_LESS_EQ:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), (left->as_int() <= right->as_int() ? "true" : "false"));
			break;
		case TT_GREATER:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), (left->as_int() > right->as_int() ? "true" : "false"));
			break;
		case TT_GREATER_EQ:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), (left->as_int() >= right->as_int() ? "true" : "false"));
			break;
		case TT_ADD:
			if (left->is_string() && right->is_string()) {
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), left->as_string() + right->as_string());
			}
			else if (left->is_string() && right->is_char()) {
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), left->as_string() + right->as_char());
			}
			else if (left->is_char() && right->is_string()) {
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), std::string(1, left->as_char()) + right->as_string());
			}
			else if (left->is_char() && right->is_int()) {
				char result = static_cast<char>(left->as_char() + right->as_int());
				expression_result = std::make_shared<runtime_value>(data_type(DT_CHAR), std::string(1, result));
			}
			else if (left->is_int() && right->is_char()) {
				char result = static_cast<char>(left->as_int() + right->as_char());
				expression_result = std::make_shared<runtime_value>(data_type(DT_CHAR), std::string(1, result));
			}
			else if (left->is_char() && right->is_char()) {
				char result = static_cast<char>(left->as_char() + right->as_char());
				expression_result = std::make_shared<runtime_value>(data_type(DT_CHAR), std::string(1, result));
			}
			else if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(left->as_float() + right->as_float()));
				}
				else {
					expression_result = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(left->as_int() + right->as_int()));
				}
			}
			else {
				throw_exception("Unsupported types for addition", expr->position);
			}
			break;
		case TT_SUBTRACT:
			if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(left->as_float() - right->as_float()));
				}
				else {
					expression_result = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(left->as_int() - right->as_int()));
				}
			}
			else if (left->is_char() && right->is_char()) {
				char result = static_cast<char>(left->as_char() - right->as_char());
				expression_result = std::make_shared<runtime_value>(data_type(DT_CHAR), std::string(1, result));
			}
			else {
				throw_exception("Unsupported types for subtraction", expr->position);
			}
			break;
		case TT_MULTIPLY:
			if (left->is_string() && right->is_int()) {
				int count = right->as_int();
				std::string result;
				for (int i = 0; i < count; ++i) result += left->as_string();
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), result);
			}
			else if (left->is_char() && right->is_int()) {
				int count = right->as_int();
				std::string result(count, left->as_char());
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), result);
			}
			else if (left->is_int() && right->is_string()) {
				int count = left->as_int();
				std::string result;
				for (int i = 0; i < count; ++i) result += right->as_string();
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), result);
			}
			else if (left->is_int() && right->is_char()) {
				int count = left->as_int();
				std::string result(count, right->as_char());
				expression_result = std::make_shared<runtime_value>(data_type(DT_STRING), result);
			}
			else if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(left->as_float() * right->as_float()));
				}
				else {
					expression_result = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(left->as_int() * right->as_int()));
				}
			}
			else {
				throw_exception("Multiplication only supports numeric types, string * int, int * string, char * int, or int * char", expr->position);
			}
			break;
		case TT_DIVIDE:
			if (right->is_number() && ((right->is_float() && right->as_float() == 0.0) || (right->is_int() && right->as_int() == 0))) {
				throw_exception("Division by zero", expr->position);
				return;
			}
			if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(left->as_float() / right->as_float()));
				}
				else {
					expression_result = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(left->as_int() / right->as_int()));
				}
			}
			else {
				throw_exception("Unsupported types for division", expr->position);
			}
			break;
		case TT_MODULO:
			if (right->is_number() && right->as_int() == 0) {
				throw_exception("Division by zero", expr->position);
				return;
			}
			if (left->is_int() && right->is_int()) {
				expression_result = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(left->as_int() % right->as_int()));
			}
			else {
				throw_exception("Modulo only supports integer types", expr->position);
			}
			break;
		case TT_AND:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL),
				(left->as_bool() && right->as_bool()) ? "true" : "false");
			break;
		case TT_OR:
			expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL),
				(left->as_bool() || right->as_bool()) ? "true" : "false");
			break;
		default:
			throw_exception("Unknown operator", expr->position);
		}
		break;
	}
}
void interpreter_visitor::visit(std::shared_ptr<assignment_expression> expr) {
	// assignment to variable
	if (auto id_expr = std::dynamic_pointer_cast<identifier_expression>(expr->left)) {
		expr->right->accept(*this);
		auto right = std::make_shared<runtime_value>(*expression_result);
		set_var(id_expr->identifier, right);
		expression_result = right;
		return;
	}
	// assignment to member: obj.member = value
	if (auto member_expr = std::dynamic_pointer_cast<member_expression>(expr->left)) {
		member_expr->object->accept(*this);
		auto obj = std::make_shared<runtime_value>(*expression_result);
		expr->right->accept(*this);
		auto right = std::make_shared<runtime_value>(*expression_result);
		obj->set_member(member_expr->member, right);
		// if the object is a variable, update it in the environment
		if (auto obj_id = std::dynamic_pointer_cast<identifier_expression>(member_expr->object)) {
			set_var(obj_id->identifier, obj);
		}
		expression_result = right;
		return;
	}
}
void interpreter_visitor::visit(std::shared_ptr<member_expression> expr) {
	// evaluate the object expression
	expr->object->accept(*this);
	auto obj = std::make_shared<runtime_value>(*expression_result);

	// get the member value
	auto member_ptr = obj->get_member(expr->member);
	if (!member_ptr) {
		throw_exception("Member \"" + expr->member + "\" not found", expr->position);
	}
	expression_result = member_ptr;
}
void interpreter_visitor::visit(std::shared_ptr<address_of_expression> expr) {
	expr->value->accept(*this);
	expression_result = std::make_shared<runtime_value>(expression_result);
}
void interpreter_visitor::visit(std::shared_ptr<unary_expression> expr) {
	expr->operand->accept(*this);
	auto operand = std::make_shared<runtime_value>(*expression_result);

	switch (expr->oparator) {
	case TT_INCREMENT: {
		auto id_expr = std::dynamic_pointer_cast<identifier_expression>(expr->operand);
		if (!id_expr) throw_exception("Cannot increment non-variable", expr->position);
		auto var = get_var(id_expr->identifier);
		if (var->is_int()) {
			var = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(var->as_int() + 1));
		}
		else if (var->is_float()) {
			var = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(var->as_float() + 1.0));
		}
		else {
			throw_exception("Increment only supported for numeric types", expr->position);
		}
		set_var(id_expr->identifier, var);
		expression_result = var;
		break;
	}
	case TT_DECREMENT: {
		auto id_expr = std::dynamic_pointer_cast<identifier_expression>(expr->operand);
		if (!id_expr) throw_exception("Cannot decrement non-variable", expr->position);
		auto var = get_var(id_expr->identifier);
		if (var->is_int()) {
			var = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(var->as_int() - 1));
		}
		else if (var->is_float()) {
			var = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(var->as_float() - 1.0));
		}
		else {
			throw_exception("Decrement only supported for numeric types", expr->position);
		}
		set_var(id_expr->identifier, var);
		expression_result = var;
		break;
	}
	case TT_SUBTRACT: {
		if (operand->is_int()) {
			expression_result = std::make_shared<runtime_value>(data_type(DT_I32), std::to_string(-operand->as_int()));
		}
		else if (operand->is_float()) {
			expression_result = std::make_shared<runtime_value>(data_type(DT_FLOAT), std::to_string(-operand->as_float()));
		}
		else {
			throw_exception("Unary minus only supported for numeric types", expr->position);
		}
		break;
	}
	case TT_NOT: {
		expression_result = std::make_shared<runtime_value>(data_type(DT_BOOL), operand->as_bool() ? "false" : "true");
		break;
	}
	default:
		throw_exception("Unknown unary operator", expr->position);
	}
}
void interpreter_visitor::visit(std::shared_ptr<index_expression> expr) {
	expr->base->accept(*this);
	auto base = std::make_shared<runtime_value>(*expression_result);
	expr->indexer->accept(*this);
	auto index = std::make_shared<runtime_value>(*expression_result);
	if (!base->is_string()) {
		throw_exception("Indexing only supported for strings", expr->position);
	}
	int idx = index->as_int();
	const std::string& str = base->as_string();
	char ch = (idx < 0 || idx >= static_cast<int>(str.length())) ? '\0' : str[idx];
	expression_result = std::make_shared<runtime_value>(data_type(DT_CHAR), std::string(1, ch));
}
void interpreter_visitor::visit(std::shared_ptr<identifier_expression> expr) {
	expression_result = get_var(expr->identifier);
}
void interpreter_visitor::visit(std::shared_ptr<this_expression> expr) {
	if (!this_object) {
		throw_exception("'this' is null", expr->position);
		return;
	}
	expression_result = this_object;
}
void interpreter_visitor::visit(std::shared_ptr<cast_expression> expr) {
	expr->expression->accept(*this);
	if (expr->expression->type()->is_pointer()) {
		// this is janky, but the whole interpreter is just for testing anyway
		expression_result = std::make_shared<runtime_value>(data_type(DT_I32), expression_result->as_string());
	}
}
void interpreter_visitor::visit(std::shared_ptr<initializer_list> init_list) {
	// handle initializer list for custom types
	expression_result = std::make_shared<runtime_value>(data_type(DT_UNKNOWN), "");
}
void interpreter_visitor::visit(std::shared_ptr<function_call> func_call) {
	if (func_call->is_method()) {
		auto member_expr = std::dynamic_pointer_cast<member_expression>(func_call->callee);
		if (!member_expr) {
			throw_exception("Method call expected", func_call->position);
		}
		// resolve the object
		member_expr->object->accept(*this);
		auto obj_ptr = std::make_shared<runtime_value>(*expression_result);
		set_this(obj_ptr);
		enter_function(func_call->declaration, func_call->args);
		remove_this();
		return;
	}
	if (func_call->declaration && func_call->declaration->is_constructor) {
		// Allocate object of the correct type
		auto custom = std::dynamic_pointer_cast<custom_type>(func_call->declaration->return_type);
		auto obj = std::make_shared<runtime_value>(*custom, "");

		// Set 'this' context for constructor
		set_this(obj);

		// Call the constructor function
		enter_function(func_call->declaration, func_call->args);

		// Remove 'this' context
		remove_this();

		// Return the constructed object
		expression_result = obj;
		return;
	}
	if (func_call->identifier == "Print") {
		func_call->args[0]->accept(*this);
		auto val = std::make_shared<runtime_value>(*expression_result);
		std::cout << val->as_string();
		std::cout.flush();
		expression_result = std::make_shared<runtime_value>(data_type(DT_VOID), "");
		return;
	}
	enter_function(func_call->declaration, func_call->args);
}
void interpreter_visitor::visit(std::shared_ptr<literal> literal) {
	expression_result = std::make_shared<runtime_value>(literal->primitive, literal->value);
}
void interpreter_visitor::visit(std::shared_ptr<block_statement> block) {
	push_scope();
	for (int i = 0; i < block->body.size(); i++) {
		block->body[i]->accept(*this);
	}
	pop_scope();
}
void interpreter_visitor::visit(std::shared_ptr<if_statement> if_stmt) {
	if_stmt->condition->accept(*this);
	if (expression_result->as_bool()) {
		if_stmt->then_block->accept(*this);
	}
	else if (if_stmt->else_block) {
		if_stmt->else_block->accept(*this);
	}
}
void interpreter_visitor::visit(std::shared_ptr<inline_if> inline_if) {
	inline_if->condition->accept(*this);
	if (expression_result->as_bool()) {
		inline_if->statement->accept(*this);
	}
}
void interpreter_visitor::visit(std::shared_ptr<for_loop> for_loop) {
	push_scope();
	if (for_loop->initializer) {
		for_loop->initializer->accept(*this);
	}
	while (true) {
		for_loop->condition->accept(*this);
		if (!expression_result->as_bool()) {
			break;
		}
		for_loop->body->accept(*this);
		if (for_loop->increment) {
			for_loop->increment->accept(*this);
		}
	}
	pop_scope();
}
void interpreter_visitor::visit(std::shared_ptr<while_loop> while_loop) {
	push_scope();
	while (true) {
		while_loop->condition->accept(*this);
		if (!expression_result->as_bool()) {
			break;
		}
		while_loop->body->accept(*this);
	}
	pop_scope();
}
void interpreter_visitor::visit(std::shared_ptr<return_statement> ret_stmt) {
	if (ret_stmt->value) {
		ret_stmt->value->accept(*this);
		throw function_return(*expression_result);
	}
	else {
		throw function_return(runtime_value(data_type(DT_VOID), ""));
	}
}
void interpreter_visitor::visit(std::shared_ptr<return_if> ret_stmt) {
	ret_stmt->condition->accept(*this);
	if (expression_result->as_bool()) {
		if (ret_stmt->value) {
			ret_stmt->value->accept(*this);
			throw function_return(*expression_result);
		}
		else {
			throw function_return(runtime_value(data_type(DT_VOID), ""));
		}
	}
}

void interpreter_visitor::push_scope() {
	variables.emplace_back();
}
void interpreter_visitor::pop_scope() {
	if (!variables.empty()) {
		variables.pop_back();
	}
}
void interpreter_visitor::add_var(const std::string& identifier, std::shared_ptr<runtime_value> value) {
	if (variables.empty()) {
		variables.emplace_back();
	}
	auto& current = variables.back();
	if (current.count(identifier)) {
		throw_exception("Variable \"" + identifier + "\" already declared in this scope", position{ 0, 0 });
	}
	current[identifier] = value;
}
std::shared_ptr<runtime_value> interpreter_visitor::get_var(const std::string& identifier) {
	// if were in a method, we should check the current types fields first
	if (this_object) {
		auto member_ptr = this_object->get_member(identifier);
		if (member_ptr) return member_ptr;
	}
	// otherwise check globals
	for (auto it = variables.rbegin(); it != variables.rend(); ++it) {
		auto found = it->find(identifier);
		if (found != it->end()) return found->second;
	}
	throw_exception("Variable \"" + identifier + "\" not found", position{ 0, 0 });
	return nullptr; // unreachable, but avoids compiler warning
}
void interpreter_visitor::set_var(const std::string& identifier, std::shared_ptr<runtime_value> value) {
	for (auto it = variables.rbegin(); it != variables.rend(); ++it) {
		if (it->count(identifier)) {
			(*it)[identifier] = value;
			return;
		}
	}
	throw_exception("Variable \"" + identifier + "\" not found", position{ 0, 0 });
}

void interpreter_visitor::set_this(std::shared_ptr<runtime_value> obj) {
	this_object = obj;
}
void interpreter_visitor::remove_this() {
	this_object = nullptr;
}

void interpreter_visitor::enter_function(std::shared_ptr<function_declaration> func, std::vector<std::shared_ptr<expression>> args) {
	function_stack.push_back(func);
	push_scope();
	// add function parameters to scope
	for (int i = 0; i < func->parameters.size(); i++) {
		args[i]->accept(*this);
		add_var(func->parameters[i]->identifier, std::make_shared<runtime_value>(*expression_result));
	}
	try {
		func->body->accept(*this);
		// no return encountered, set to void
		expression_result = std::make_shared<runtime_value>(data_type(DT_VOID), "");
	}
	catch (const function_return& ret) {
		expression_result = std::make_shared<runtime_value>(ret.value);
	}
	pop_scope();
	function_stack.pop_back();
}
void interpreter_visitor::throw_exception(std::string message, position pos) {
#ifdef _DEBUG
	throw("Runtime exception occured during process execution");
#endif
	std::cerr << "Runtime exception occurred: " << message << "\n";
	std::cerr << "At line: " << pos.line << ", column: " << pos.column << "\n\n";
	std::cerr << "Call stack:" << "\n";
	for (int i = 0; i < function_stack.size(); i++) {
		std::cerr << function_stack[i]->identifier << "()" << "\n";
	}
	exit(1);
}