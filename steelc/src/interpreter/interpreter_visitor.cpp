#include "interpreter_visitor.h"

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <conio.h>
#include <Windows.h>

#include "classes/runtime_exception.h"
#include "classes/break_out.h"
#include "../parser/ast/ast.h"
#include "../parser/types/custom_type.h"

void interpreter_visitor::begin(std::shared_ptr<function_declaration> entry_point) {
	std::cout << "Starting execution...\n" << std::endl;

	try {
		enter_function(entry_point, {});

		// execution finished successfully
		std::cout << "\nExecution finished successfully.\nExit code: " << expression_result->as_int() << std::endl;
	}
	catch (const runtime_exception& e) {
		std::cerr << "\nRuntime Exception Thrown: " << e.what() << std::endl;
		std::cerr << "At line: " << e.position.line << ", column: " << e.position.column << std::endl;

		std::cerr << "\nExecution failed." << std::endl;
		return;
	}
}

void interpreter_visitor::visit(std::shared_ptr<function_declaration> func) {
	func->body->accept(*this);
}
void interpreter_visitor::visit(std::shared_ptr<variable_declaration> var) {
	if (var->initializer) {
		// handle initializer list for custom types
		if (auto init_list = std::dynamic_pointer_cast<initializer_list>(var->initializer)) {
			if (init_list->is_array_initializer) {
				std::vector<std::shared_ptr<runtime_value>> elements;
				for (auto& val : init_list->values) {
					val->accept(*this);
					elements.push_back(new_val(expression_result));
				}
				expression_result = new_array(var->type, elements);
				add_var(var->identifier, expression_result);
				return;
			}
			// check if type is custom
			auto custom = std::dynamic_pointer_cast<custom_type>(var->type);
			if (custom && custom->declaration) {
				auto obj = new_val(var->type, "");
				// assign initializer values to fields
				for (size_t i = 0; i < init_list->values.size(); i++) {
					auto& member_decl = custom->declaration->fields[i];
					init_list->values[i]->accept(*this);
					obj->set_member(member_decl->identifier, new_val(expression_result));
				}
				add_var(var->identifier, obj);
				return;
			}
		}
		var->initializer->accept(*this);
		add_var(var->identifier, new_val(expression_result));
	}
	else {
		// handle default initialization for custom types
		auto custom = std::dynamic_pointer_cast<custom_type>(var->type);
		if (custom && custom->declaration) {
			auto obj = new_val(var->type, "");
			for (auto& member : custom->declaration->fields) {
				obj->set_member(member->identifier, new_val(member->type, ""));
			}
			add_var(var->identifier, obj);
			return;
		}
		add_var(var->identifier, new_val(var->type, ""));
	}
}
void interpreter_visitor::visit(std::shared_ptr<type_declaration> decl) {

}
void interpreter_visitor::visit(std::shared_ptr<expression_statement> expr) {
	expr->expr->accept(*this);
}
void interpreter_visitor::visit(std::shared_ptr<binary_expression> expr) {
	expr->left->accept(*this);
	auto left = new_val(expression_result);
	expr->right->accept(*this);
	auto right = new_val(expression_result);
	switch (expr->oparator) {
	default:
		switch (expr->oparator) {
		case TT_EQUAL:
			expression_result = new_val(to_data_type(DT_BOOL), (*left == *right ? "true" : "false"));
			break;
		case TT_NOT_EQUAL:
			expression_result = new_val(to_data_type(DT_BOOL), (*left != *right ? "true" : "false"));
			break;
		case TT_LESS:
			expression_result = new_val(to_data_type(DT_BOOL), (*left < *right ? "true" : "false"));
			break;
		case TT_LESS_EQ:
			expression_result = new_val(to_data_type(DT_BOOL), (left->as_int() <= right->as_int() ? "true" : "false"));
			break;
		case TT_GREATER:
			expression_result = new_val(to_data_type(DT_BOOL), (left->as_int() > right->as_int() ? "true" : "false"));
			break;
		case TT_GREATER_EQ:
			expression_result = new_val(to_data_type(DT_BOOL), (left->as_int() >= right->as_int() ? "true" : "false"));
			break;
		case TT_ADD:
			if (left->is_string() && right->is_string()) {
				expression_result = new_val(to_data_type(DT_STRING), left->as_string() + right->as_string());
			}
			else if (left->is_string() && right->is_char()) {
				expression_result = new_val(to_data_type(DT_STRING), left->as_string() + right->as_char());
			}
			else if (left->is_char() && right->is_string()) {
				expression_result = new_val(to_data_type(DT_STRING), std::string(1, left->as_char()) + right->as_string());
			}
			else if (left->is_char() && right->is_int()) {
				char result = static_cast<char>(left->as_char() + right->as_int());
				expression_result = new_val(to_data_type(DT_CHAR), std::string(1, result));
			}
			else if (left->is_int() && right->is_char()) {
				char result = static_cast<char>(left->as_int() + right->as_char());
				expression_result = new_val(to_data_type(DT_CHAR), std::string(1, result));
			}
			else if (left->is_char() && right->is_char()) {
				char result = static_cast<char>(left->as_char() + right->as_char());
				expression_result = new_val(to_data_type(DT_CHAR), std::string(1, result));
			}
			else if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = new_val(to_data_type(DT_FLOAT), std::to_string(left->as_float() + right->as_float()));
				}
				else {
					expression_result = new_val(to_data_type(DT_I32), std::to_string(left->as_int() + right->as_int()));
				}
			}
			else {
				throw_exception("Unsupported types for addition", expr->position);
			}
			break;
		case TT_SUBTRACT:
			if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = new_val(to_data_type(DT_FLOAT), std::to_string(left->as_float() - right->as_float()));
				}
				else {
					expression_result = new_val(to_data_type(DT_I32), std::to_string(left->as_int() - right->as_int()));
				}
			}
			else if (left->is_char() && right->is_char()) {
				char result = static_cast<char>(left->as_char() - right->as_char());
				expression_result = new_val(to_data_type(DT_CHAR), std::string(1, result));
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
				expression_result = new_val(to_data_type(DT_STRING), result);
			}
			else if (left->is_char() && right->is_int()) {
				int count = right->as_int();
				std::string result(count, left->as_char());
				expression_result = new_val(to_data_type(DT_STRING), result);
			}
			else if (left->is_int() && right->is_string()) {
				int count = left->as_int();
				std::string result;
				for (int i = 0; i < count; ++i) result += right->as_string();
				expression_result = new_val(to_data_type(DT_STRING), result);
			}
			else if (left->is_int() && right->is_char()) {
				int count = left->as_int();
				std::string result(count, right->as_char());
				expression_result = new_val(to_data_type(DT_STRING), result);
			}
			else if (left->is_number() && right->is_number()) {
				if (left->is_float() || right->is_float()) {
					expression_result = new_val(to_data_type(DT_FLOAT), std::to_string(left->as_float() * right->as_float()));
				}
				else {
					expression_result = new_val(to_data_type(DT_I32), std::to_string(left->as_int() * right->as_int()));
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
					expression_result = new_val(to_data_type(DT_FLOAT), std::to_string(left->as_float() / right->as_float()));
				}
				else {
					expression_result = new_val(to_data_type(DT_I32), std::to_string(left->as_int() / right->as_int()));
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
				expression_result = new_val(to_data_type(DT_I32), std::to_string(left->as_int() % right->as_int()));
			}
			else {
				throw_exception("Modulo only supports integer types", expr->position);
			}
			break;
		case TT_AND:
			expression_result = new_val(to_data_type(DT_BOOL),
				(left->as_bool() && right->as_bool()) ? "true" : "false");
			break;
		case TT_OR:
			expression_result = new_val(to_data_type(DT_BOOL),
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
		auto right = new_val(expression_result->type, expression_result->value);
		set_var(id_expr->identifier, right);
		expression_result = right;
		return;
	}
	// assignment to member: obj.member = value
	if (auto member_expr = std::dynamic_pointer_cast<member_expression>(expr->left)) {
		member_expr->object->accept(*this);
		auto& obj = *expression_result;
		expr->right->accept(*this);
		auto right = new_val(expression_result->type, expression_result->value);
		obj.set_member(member_expr->member, right);
		expression_result = right;
		return;
	}
}
void interpreter_visitor::visit(std::shared_ptr<member_expression> expr) {
	if (auto id_expr = std::dynamic_pointer_cast<identifier_expression>(expr->object)) {
		// if the lhs is an identifier we can check if its an enum before
		// accepting it which would throw an error since it might not be
		// a variable
		if (id_expr->id_type == IDENTIFIER_ENUM) {
			auto& enum_decl = id_expr->enum_declaration;

			for (auto& option : enum_decl->options) {
				if (option.identifier == expr->member) {
					expression_result = new_val(enum_decl->type(), option.identifier);
					return;
				}
			}

			throw_exception("Enum member \"" + expr->member + "\" not found", expr->position);
			return;
		}
	}

	// evaluate the object expression
	expr->object->accept(*this);
	auto& obj = expression_result;

	// dereference ONCE if the object is a pointer
	if (obj->is_pointer()) {
		obj = obj->pointee;
		if (!obj) {
			throw_exception("Dereference of null pointer", expr->position);
			return;
		}
	}

	// get the member value
	auto member_ptr = obj->get_member(expr->member);
	if (!member_ptr) {
		throw_exception("Member \"" + expr->member + "\" not found", expr->position);
	}
	expression_result = member_ptr;
}
void interpreter_visitor::visit(std::shared_ptr<address_of_expression> expr) {
	expr->value->accept(*this);
	expression_result = new_pointer(expression_result);
}
void interpreter_visitor::visit(std::shared_ptr<deref_expression> expr) {
	expr->value->accept(*this);
	expression_result = expression_result->pointee;
}
void interpreter_visitor::visit(std::shared_ptr<unary_expression> expr) {
	expr->operand->accept(*this);
	auto operand = new_val(expression_result->type, expression_result->value);

	switch (expr->oparator) {
	case TT_INCREMENT: {
		auto id_expr = std::dynamic_pointer_cast<identifier_expression>(expr->operand);
		if (!id_expr) throw_exception("Cannot increment non-variable", expr->position);
		auto var = get_var(id_expr->identifier);
		if (var->is_int()) {
			var = new_val(to_data_type(DT_I32), std::to_string(var->as_int() + 1));
		}
		else if (var->is_float()) {
			var = new_val(to_data_type(DT_FLOAT), std::to_string(var->as_float() + 1.0));
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
			var = new_val(to_data_type(DT_I32), std::to_string(var->as_int() - 1));
		}
		else if (var->is_float()) {
			var = new_val(to_data_type(DT_FLOAT), std::to_string(var->as_float() - 1.0));
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
			expression_result = new_val(to_data_type(DT_I32), std::to_string(-operand->as_int()));
		}
		else if (operand->is_float()) {
			expression_result = new_val(to_data_type(DT_FLOAT), std::to_string(-operand->as_float()));
		}
		else {
			throw_exception("Unary minus only supported for numeric types", expr->position);
		}
		break;
	}
	case TT_NOT: {
		expression_result = new_val(to_data_type(DT_BOOL), operand->as_bool() ? "false" : "true");
		break;
	}
	default:
		throw_exception("Unknown unary operator", expr->position);
	}
}
void interpreter_visitor::visit(std::shared_ptr<index_expression> expr) {
	expr->base->accept(*this);
	auto base = new_val(expression_result->type, expression_result->value);
	expr->indexer->accept(*this);
	auto index = new_val(expression_result->type, expression_result->value);
	if (base->is_string()) {
		int idx = index->as_int();
		const std::string& str = base->as_string();
		char ch = (idx < 0 || idx >= static_cast<int>(str.length())) ? '\0' : str[idx];
		expression_result = new_val(to_data_type(DT_CHAR), std::string(1, ch));
		return;
	}
	else if (base->is_array()) {
		auto arr = base->as_array();
		int idx = index->as_int();
		if (idx < 0 || idx >= static_cast<int>(arr.size())) {
			throw_exception("Array index out of bounds", expr->position);
		}
		expression_result = arr[idx];
		return;
	}
	else {
		throw_exception("Indexing only supported for strings and arrays", expr->position);
	}
}
void interpreter_visitor::visit(std::shared_ptr<identifier_expression> expr) {
	if (expr->id_type == IDENTIFIER_VARIABLE) {
		expression_result = get_var(expr->identifier);
	}
	else {
		throw_exception("Unsupported identifier type in interpreter", expr->position);
	}
}
void interpreter_visitor::visit(std::shared_ptr<this_expression> expr) {
	if (!this_object) {
		throw_exception("'this' is null", expr->position);
		return;
	}
	expression_result = this_object;
}
void interpreter_visitor::visit(std::shared_ptr<cast_expression> expr) {
	expr->expr->accept(*this);
	if (expr->expr->type()->is_pointer()) {
		// this is janky, but the whole interpreter is just for testing anyway
		expression_result = new_val(to_data_type(DT_I32), expression_result->as_string());
	}
}
void interpreter_visitor::visit(std::shared_ptr<initializer_list> init_list) {
	// handle initializer list for custom types
	expression_result = new_val(to_data_type(DT_UNKNOWN), "");
}
void interpreter_visitor::visit(std::shared_ptr<function_call> func_call) {
	if (func_call->is_method()) {
		auto member_expr = std::dynamic_pointer_cast<member_expression>(func_call->callee);
		if (!member_expr) {
			throw_exception("Method call expected", func_call->position);
		}
		// resolve the object
		member_expr->object->accept(*this);
		std::shared_ptr<runtime_value> obj_ptr = expression_result;

		// if obj_ptr is a pointer, dereference to get the actual object
		if (obj_ptr->is_pointer() && obj_ptr->pointee) {
			obj_ptr = obj_ptr->pointee;
		}

		// look up the method in the object's vftable
		auto vft_it = obj_ptr->vftable.find(func_call->identifier);
		if (vft_it == obj_ptr->vftable.end() || !vft_it->second->body) {
			if (obj_ptr->is_pointer()) {
				throw_exception("Method call on pointer", func_call->position);
				return;
			}
			throw_exception("Virtual method not implemented: " + func_call->identifier, func_call->position);
			return;
		}

		// call the resolved method
		enter_method(vft_it->second, func_call->args, obj_ptr);
		return;
	}
	if (func_call->declaration && func_call->declaration->is_constructor) {
		// allocate object of the correct type
		auto custom = std::dynamic_pointer_cast<custom_type>(func_call->declaration->return_type);
		auto obj = new_val(custom);
		if (custom && custom->declaration) {
			for (auto& member : custom->declaration->fields) {
				obj->set_member(member->identifier, new_val(member->type));
			}
			build_vftable(obj, custom);
		}

		// call the constructor function
		enter_method(func_call->declaration, func_call->args, obj);

		// return the constructed object
		expression_result = obj;
		return;
	}
	if (func_call->identifier == "Print") {
		func_call->args[0]->accept(*this);
		auto val = new_val(expression_result);
		std::cout << val->as_string();
		std::cout.flush();
		expression_result = new_val(to_data_type(DT_VOID), "");
		return;
	}
	if (func_call->identifier == "Read") {
		std::string input;
		std::getline(std::cin, input);
		expression_result = new_val(to_data_type(DT_STRING), input);
		return;
	}
	if (func_call->identifier == "ReadKey") {
		char ch = _getch();
		expression_result = new_val(to_data_type(DT_CHAR), std::string(1, ch));
		return;
	}
	if (func_call->identifier == "ReadInt") {
		std::string input;
		std::getline(std::cin, input);
		expression_result = new_val(to_data_type(DT_I32), input);
		return;
	}
	if (func_call->identifier == "Wait") {
		func_call->args[0]->accept(*this);
		auto seconds = new_val(expression_result->type, expression_result->value);
		if (!seconds->is_number()) {
			throw_exception("Wait function expects a numeric argument", func_call->position);
			return;
		}
		int ms = static_cast<int>(seconds->as_float() * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		return;
	}
	if (func_call->identifier == "SetConsolePos") {
		func_call->args[0]->accept(*this);
		auto x = new_val(expression_result->type, expression_result->value);
		func_call->args[1]->accept(*this);
		auto y = new_val(expression_result->type, expression_result->value);
		if (!x->is_int() || !y->is_int()) {
			throw_exception("SetConsolePos function expects a numeric argument", func_call->position);
			return;
		}
		COORD coord{
			static_cast<SHORT>(x->as_int()),
			static_cast<SHORT>(y->as_int())
		};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		return;
	}
	if (func_call->identifier == "GetKey") {
		func_call->args[0]->accept(*this);
		auto x = new_val(expression_result->type, expression_result->value);
		if (!x->is_char()) {
			throw_exception("GetKey expects a character", func_call->position);
			return;
		}
		char ch = _getch();
		expression_result = new_val(to_data_type(DT_BOOL), (ch == x->as_char()) ? "true" : "false");
		return;
	}
	enter_function(func_call->declaration, func_call->args);
}
void interpreter_visitor::visit(std::shared_ptr<literal> literal) {
	expression_result = new_val(to_data_type(literal->primitive), literal->value);
}
void interpreter_visitor::visit(std::shared_ptr<code_block> block) {
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
	else if (if_stmt->else_node) {
		if_stmt->else_node->accept(*this);
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
		try {
			for_loop->body->accept(*this);
		}
		catch (const break_out&) {
			break;
		}
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
		try {
			while_loop->body->accept(*this);
		}
		catch (const break_out&) {
			break;
		}
	}
	pop_scope();
}
void interpreter_visitor::visit(std::shared_ptr<return_statement> ret_stmt) {
	if (ret_stmt->is_conditional()) {
		ret_stmt->condition->accept(*this);
		if (!expression_result->as_bool()) {
			// condition is false, do not return
			return;
		}
	}
	if (ret_stmt->value) {
		ret_stmt->value->accept(*this);
		throw function_return(*expression_result);
	}
	else {
		throw function_return(runtime_value(to_data_type(DT_VOID), ""));
	}
}
void interpreter_visitor::visit(std::shared_ptr<break_statement> brk_stmt) {
 	if (brk_stmt->is_conditional()) {
		brk_stmt->condition->accept(*this);
		if (!expression_result->as_bool()) {
			// condition is false, do not break
			return;
		}
	}
	throw break_out();
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
	if (this_object) {
		auto member_ptr = this_object->get_member(identifier);
		if (member_ptr) {
			this_object->set_member(identifier, value);
			return;
		}
	}
	auto var = get_var(identifier);
	if (var) {
		*var = *value;
		return;
	}
	throw_exception("Variable \"" + identifier + "\" not found", position{ 0, 0 });
}

val_ptr interpreter_visitor::new_val(const type_ptr type) {
	return std::make_shared<runtime_value>(type);
}
val_ptr interpreter_visitor::new_val(const type_ptr type, const std::string& value) {
	return std::make_shared<runtime_value>(type, value);
}
val_ptr interpreter_visitor::new_val(val_ptr other) {
	return std::make_shared<runtime_value>(*other);
}
val_ptr interpreter_visitor::new_pointer(std::shared_ptr<runtime_value> pointee) {
	return std::make_shared<runtime_value>(pointee);
}
val_ptr interpreter_visitor::new_array(const type_ptr elem_type, const std::vector<std::shared_ptr<runtime_value>>& elements) {
	return std::make_shared<runtime_value>(elem_type, elements);
}

void interpreter_visitor::build_vftable(std::shared_ptr<runtime_value> obj, std::shared_ptr<custom_type> type) {
	if (!type->declaration) return;

	// handle inheritance
	for (const auto& base_type : type->declaration->base_types) {
		auto base_custom = std::dynamic_pointer_cast<custom_type>(base_type);
		if (base_custom) {
			build_vftable(obj, base_custom);
		}
	}
	// add override methods to vftable
	for (auto& method : type->declaration->methods) {
		obj->vftable[method->identifier] = method;
	}
}
void interpreter_visitor::set_this(std::shared_ptr<runtime_value> obj) {
	this_object = obj;
	this_stack.push_back(obj);
}
void interpreter_visitor::remove_this() {
	this_stack.pop_back();
	if (this_stack.empty()) {
		this_object = nullptr;
		return;
	}
	this_object = this_stack.back();
}

void interpreter_visitor::enter_function(std::shared_ptr<function_declaration> func, std::vector<std::shared_ptr<expression>> args) {
	function_stack.push_back(func);
	push_scope();
	// add function parameters to scope
	for (int i = 0; i < func->parameters.size(); i++) {
		args[i]->accept(*this);
		add_var(func->parameters[i]->identifier, new_val(expression_result->type, expression_result->value));
	}
	try {
		func->body->accept(*this);
		// no return encountered, set to void
		expression_result = new_val(to_data_type(DT_VOID), "");
	}
	catch (const function_return& ret) {
		expression_result = new_val(ret.value.type, ret.value.value);
	}
	pop_scope();
	function_stack.pop_back();
}
void interpreter_visitor::enter_method(std::shared_ptr<function_declaration> func, std::vector<std::shared_ptr<expression>> args, std::shared_ptr<runtime_value> object) {
	function_stack.push_back(func);
	push_scope();
	// add function parameters to scope
	for (int i = 0; i < func->parameters.size(); i++) {
		args[i]->accept(*this);
		add_var(func->parameters[i]->identifier, new_val(expression_result->type, expression_result->value));
	}

	set_this(object);

	try {
		if (!func->body) {
			throw_exception("Function has no body", position{ 0, 0 });
			return;
		}
		func->body->accept(*this);
		// no return encountered, set to void
		expression_result = new_val(to_data_type(DT_VOID), "");
	}
	catch (const function_return& ret) {
		expression_result = new_val(ret.value.type, ret.value.value);
	}

	remove_this();
	pop_scope();
	function_stack.pop_back();
}
void interpreter_visitor::throw_exception(std::string message, position pos) {
	throw runtime_exception(message, pos);
}