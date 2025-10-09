#include "parser.h"

#include "types/type_handle.h"
#include "types/primitive_type.h"
#include "types/custom_type.h"
#include "parser_utils.h"
#include "../lexer/token_utils.h"
#include "../error/error_catalog.h"

void parser::parse() {
	while (!is_at_end()) {
		ast_node_ptr ast = parse_declaration();
		if (ast) {
			unit->declarations.push_back(ast);
		}
		else {
			synchronize();
		}
	}
}
ast_node_ptr parser::parse_declaration() {
	// module declaration
	if (match(TT_MODULE)) {
		if (!match(TT_IDENTIFIER)) {
			ERROR_TOKEN(ERR_MODULE_NAME_EXPECTED, peek());
			return nullptr;
		}
		token& module_name_token = previous();

		if (!match(TT_LBRACE)) {
			ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
			return nullptr;
		}
		
		std::vector<ast_node_ptr> declarations;
		while (!is_at_end() && !check(TT_RBRACE)) {
			ast_node_ptr decl = parse_declaration();
			if (decl) {
				declarations.push_back(decl);
			}
			else {
				synchronize();
			}
		}
		if (!match(TT_RBRACE)) {
			ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
			return nullptr;
		}

		return make_ast<module_declaration>(module_name_token, module_name_token.value, declarations);
	}
	// module import
	else if (match(TT_IMPORT)) {
		std::string module_name = "";
		token& module_name_token = peek();

		bool first = true;
		do {
			if (!match(TT_IDENTIFIER)) {
				ERROR_TOKEN(ERR_MODULE_NAME_EXPECTED, peek());
				return nullptr;
			}

			if (!first) module_name += ".";
			module_name += previous().value;
			first = false;
		} while (match(TT_DOT));

		if (!match(TT_SEMICOLON)) {
			ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
			return nullptr;
		}
		
		return make_ast<import_statement>(module_name_token, module_name);
	}
	// type declaration
	else if (match({ TT_STRUCT, TT_CLASS, TT_INTERFACE })) {
		token& kind_token = previous();
		return parse_type_declaration(kind_token);
	}
	else if (match(TT_FUNC) || match(TT_OVERRIDE)) {
		return parse_function_declaration(false, previous().type == TT_OVERRIDE);
	}
	else if (match(TT_CONST) || match(TT_LET)) {
		bool is_const = previous().type == TT_CONST;
		return parse_variable_declaration(is_const);
	}
	else {
		ERROR_TOKEN(ERR_DECLARATION_EXPECTED, peek());
	}
	return nullptr;
}
ast_ptr<function_declaration> parser::parse_constructor_declaration(token& typename_token) {
	auto constructor = parse_function_declaration(true, false);
	if (constructor) {
		constructor->identifier = typename_token.value;
		constructor->is_constructor = true;
		return constructor;
	}
	return nullptr;
}
ast_ptr<function_declaration> parser::parse_function_declaration(bool is_constructor, bool is_override) {
	token identifier_token;
	if (!is_constructor) {
		if (!match(TT_IDENTIFIER)) {
			ERROR_TOKEN(ERR_ID_EXPECTED, peek());
			return nullptr;
		}
		identifier_token = previous();
	}
	
	std::vector<std::shared_ptr<generic_parameter>> generics;
	if (ENABLE_GENERICS && !is_override && !is_constructor) {
		if (match(TT_LESS)) {
			generics = parse_generics();
		}
	}

	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	std::vector<std::shared_ptr<variable_declaration>> parameters = parse_parameter_list(TT_RPAREN);

	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	ast_ptr<type_node> ret_type = nullptr;
	if (!is_override && !is_constructor) {
		if (match(TT_ARROW)) {
			ret_type = parse_type();
			if (!ret_type) {
				ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
				return nullptr;
			}
		}
	}

	if (match(TT_SEMICOLON)) {
		return make_ast<function_declaration>(identifier_token, type_handle(ret_type), identifier_token.value, parameters, nullptr, is_override);
	}

	ast_node_ptr body = parse_block();
	if (body) {
		auto decl = make_ast<function_declaration>(identifier_token, type_handle(ret_type), identifier_token.value, parameters, body, is_override);
		if (!generics.empty()) {
			decl->is_generic = true;
			decl->generics = generics;
		}
		return decl;
	}
	return nullptr;
}
ast_ptr<variable_declaration> parser::parse_variable_declaration(bool is_const) {
	ast_ptr<type_node> var_type = nullptr;
	if (match(TT_LBRACKET)) {
		token& type_token = peek();

		var_type = parse_type();
		if (!var_type) {
			ERROR_TOKEN(ERR_TYPENAME_EXPECTED, type_token);
			return nullptr;
		}

		if (!match(TT_RBRACKET)) {
			ERROR_TOKEN(ERR_RBRACKET_EXPECTED, peek());
			return nullptr;
		}
	}

	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_VAR_NAME_EXPECTED, peek());
		return nullptr;
	}
	token& identifier_token = previous();

	ast_ptr<expression> initializer = nullptr;
	if (match(TT_ASSIGN)) {
		if (check(TT_LBRACE)) {
			initializer = parse_initializer_list();
		}
		else if (check(TT_LBRACKET)) {
			initializer = parse_array_initializer();
		}
		else {
			initializer = parse_expression();
		}
	}
	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}

	auto decl = make_ast<variable_declaration>(identifier_token, is_const, var_type, identifier_token.value, initializer);
	return decl;
}
ast_ptr<type_declaration> parser::parse_type_declaration(token& kind_token) {
	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_ID_EXPECTED, peek());
		return nullptr;
	}
	token& identifier_token = previous();

	std::vector<std::shared_ptr<generic_parameter>> generics;
	if (ENABLE_GENERICS && match(TT_LESS)) {
		generics = parse_generics();
	}

	custom_type_kind type_kind = kind_token.type == TT_STRUCT ? CT_STRUCT : kind_token.type == TT_CLASS ? CT_CLASS : CT_INTERFACE;
	auto type_decl = make_ast<type_declaration>(identifier_token, type_kind, identifier_token.value);

	std::vector<type_handle> base_types;
	if (match(TT_COLON)) {
		do {
			if (!match(TT_IDENTIFIER)) {
				ERROR_TOKEN(ERR_ID_EXPECTED, peek());
				return nullptr;
			}
			auto type = parse_type();
			base_types.push_back(type_handle(type));
		} while (match(TT_COMMA));
	}
	type_decl->base_types = base_types;

	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}

	while (!is_at_end() && !check(TT_RBRACE)) {
		if (match(TT_CONSTRUCTOR)) {
			auto constructor = parse_constructor_declaration(identifier_token);
			if (constructor) type_decl->constructors.push_back(constructor);
		}
		else if (match(TT_FUNC) || match(TT_OVERRIDE)) {
			auto method = parse_function_declaration(false, previous().type == TT_OVERRIDE);
			if (method) type_decl->methods.push_back(method);
		}
		else if (match(TT_IDENTIFIER)) {
			// field declaration - might change this later
			token& field_name_token = previous();

			if (!match(TT_COLON)) {
				ERROR_TOKEN(ERR_COLON_EXPECTED, peek());
				advance();
				continue;
			}

			ast_ptr<type_node> field_type = parse_type();
			if (!field_type) {
				ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
				advance();
				continue;
			}

			if (!match(TT_SEMICOLON)) {
				ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
				advance();
				continue;
			}

			type_decl->fields.push_back(make_ast<variable_declaration>(field_name_token, /* not const */ false, field_type, field_name_token.value));
		}
		else {
			ERROR_TOKEN(ERR_MEMBER_DECLARATION_EXPECTED, peek());
			advance();
		}
	}
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}

	return type_decl;
}
ast_ptr<variable_declaration> parser::parse_parameter() {
	// parse parameter type
	ast_ptr<type_node> param_type = parse_type();
	if (!param_type) {
		ERROR_TOKEN(ERR_PARAM_TYPE_EXPECTED, peek());
		return nullptr;
	}

	// parse parameter name
	if (!match(TT_IDENTIFIER)) {
		ERROR_TOKEN(ERR_PARAM_NAME_EXPECTED, peek());
		return nullptr;
	}
	token& param_name_token = previous();

	// TODO: allow for default values in parameters

	auto param = make_ast<variable_declaration>(param_name_token, false, param_type, param_name_token.value);
	param->is_parameter = true;
	return param;
}
ast_node_ptr parser::parse_statement() {
	if (check(TT_LBRACE)) {
		// parse block also matches LBRACE, so we only check here
		return parse_block();
	}
	else if (match(TT_IF)) {
		return parse_if_statement();
	}
	else if (match(TT_FOR)) {
		return parse_for_loop();
	}
	else if (match(TT_WHILE)) {
		return parse_while_loop();
	}
	else if (match(TT_RETURN)) {
		return parse_return_statement();
	}
	return parse_expression_statement();
}
ast_ptr<code_block> parser::parse_block() {
	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}
	std::shared_ptr<code_block> block = std::make_shared<code_block>();
	while (!is_at_end() && !check(TT_RBRACE)) {
		if (match(TT_CONST) || match(TT_LET)) {
			bool is_const = previous().type == TT_CONST;
			ast_node_ptr decl = parse_variable_declaration(is_const);
			if (decl) block->statements.push_back(decl);
		}
		else {
			ast_node_ptr stmt = parse_statement();
			if (stmt) block->statements.push_back(stmt);
			else {
				synchronize();
			}
		}
	}
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}
	return block;
}
ast_ptr<if_statement> parser::parse_if_statement() {
	token& if_token = previous();
	std::shared_ptr<if_statement> if_stmt = make_ast<if_statement>(if_token);
	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto condition = parse_expression();
	if_stmt->condition = condition;
	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	ast_ptr<code_block> then_block = parse_block();
	if (then_block) if_stmt->then_block = then_block;

	if (match(TT_ELSE)) {
		if (match(TT_IF)) {
			// else if statement
			if_stmt->else_statement = parse_if_statement();
		}
		else {
			// else block
			if_stmt->else_statement = parse_block();
		}
	}

	return if_stmt;
}
ast_ptr<for_loop> parser::parse_for_loop() {
	token& for_token = previous();
	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	ast_node_ptr initializer = nullptr;
	if (match(TT_CONST) || match(TT_LET)) {
		bool is_const = previous().type == TT_CONST;
		initializer = parse_variable_declaration(is_const);
	}
	else if (!check(TT_SEMICOLON)) {
		initializer = parse_expression_statement();
	}
	else {
		// no initializer
		match(TT_SEMICOLON);
	}

	ast_ptr<expression> condition = nullptr;
	if (!check(TT_SEMICOLON)) {
		condition = parse_expression();
	}
	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}

	ast_ptr<expression> increment = nullptr;
	if (!check(TT_RPAREN)) {
		increment = parse_expression();
	}
	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	ast_node_ptr body = parse_block();

	return make_ast<for_loop>(for_token, initializer, condition, increment, body);
}
ast_ptr<while_loop> parser::parse_while_loop() {
	token& while_token = previous();
	if (!match(TT_LPAREN)) {
		ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto condition = parse_expression();
	if (!match(TT_RPAREN)) {
		ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
		return nullptr;
	}

	auto body = parse_block();

	return make_ast<while_loop>(while_token, condition, body);
}
ast_ptr<return_statement> parser::parse_return_statement() {
	token& return_token = previous();

	ast_ptr<expression> value = nullptr;
	ast_ptr<expression> condition = nullptr;

	// if a semicolon is found immediately after return, it's a bare return
	if (match(TT_SEMICOLON)) {
		return make_ast<return_statement>(return_token, nullptr);
	}

	// if theres an if after return, it's a return if (without value)
	if (match(TT_IF)) {
		if (!match(TT_LPAREN)) {
			ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
			return nullptr;
		}

		condition = parse_expression();

		if (!match(TT_RPAREN)) {
			ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
			return nullptr;
		}

		if (!match(TT_SEMICOLON)) {
			ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
			return nullptr;
		}

		return make_ast<return_statement>(return_token, nullptr, condition);
	}

	// parse the return value expression
	auto value = parse_expression();

	// return if (with value)
	if (match(TT_IF)) {
		if (!match(TT_LPAREN)) {
			ERROR_TOKEN(ERR_LPAREN_EXPECTED, peek());
			return nullptr;
		}

		condition = parse_expression();

		if (!match(TT_RPAREN)) {
			ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
			return nullptr;
		}

		if (!match(TT_SEMICOLON)) {
			ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
			return nullptr;
		}
		return make_ast<return_statement>(return_token, value, condition);
	}

	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}
	return make_ast<return_statement>(return_token, value);
}
ast_node_ptr parser::parse_expression_statement() {
	token& expr_token = peek();
	auto expr = parse_expression();
	if (match(TT_IF)) {
		auto cond = parse_expression();
		if (!match(TT_SEMICOLON)) {
			ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
			return nullptr;
		}
		return make_ast<inline_if>(expr_token, cond, expr);
	}
	if (!match(TT_SEMICOLON)) {
		ERROR_TOKEN(ERR_SEMICOLON_EXPECTED, peek());
		return nullptr;
	}
	return make_ast<expression_statement>(expr_token, expr);
}
ast_ptr<expression> parser::parse_expression() {
	return parse_binary_expression();
}
ast_ptr<expression> parser::parse_binary_expression(int precedence) {
	token& expr_token = peek();
	ast_ptr<expression> left = parse_unary_expression();
	while (true) {
		token& op = peek();
		if (!is_op(op)) {
			break; // no more operators
		}
		int op_precedence = precedence_of(op.type);
		if (op_precedence < precedence) {
			break; // no more operators with higher precedence
		}

		bool right_associative = op.type == TT_ASSIGN;

		advance(); // consume the operator

		// parse the right-hand side expression
		int next_precedence = right_associative ? op_precedence : op_precedence + 1;
		ast_ptr<expression> right = parse_binary_expression(next_precedence);

		if (op.type == TT_ASSIGN) {
			left = make_ast<assignment_expression>(expr_token, left, right);
		}
		// might change this later but it makes implementation significantly easier
		else if (op.type == TT_ASSIGN_ADD) {
			auto add_expr = make_ast<binary_expression>(expr_token, left, right, TT_PLUS);
			left = make_ast<assignment_expression>(expr_token, left, add_expr);
		}
		else if (op.type == TT_ASSIGN_SUBTRACT) {
			auto sub_expr = make_ast<binary_expression>(expr_token, left, right, TT_MINUS);
			left = make_ast<assignment_expression>(expr_token, left, sub_expr);
		}
		else {
			left = make_ast<binary_expression>(expr_token, left, right, op.type);
		}
	}
	return left;
}
ast_ptr<expression> parser::parse_unary_expression() {
	token& tok = peek();
	// address-of operator
	if (tok.type == TT_AMPERSAND) {
		advance();
		auto operand = parse_unary_expression();
		return make_ast<address_of_expression>(tok, operand);
	}
	// deref operator
	if (tok.type == TT_ASTERISK) {
		advance();
		auto operand = parse_unary_expression();
		return make_ast<deref_expression>(tok, operand);
	}
	// check for prefix unary operators (steel does not support increment/decrement prefix operators)
	if (tok.type == TT_MINUS || tok.type == TT_NOT) {
		advance();
		auto operand = parse_unary_expression();
		return make_ast<unary_expression>(tok, tok.type, operand);
	}
	// otherwise, parse primary expression
	return parse_primary_expression();
}
ast_ptr<expression> parser::parse_primary_expression() {
	ast_ptr<expression> expr = nullptr;

	if (match(TT_IDENTIFIER)) {
		// this is only used for global functions
		// for member/expression based functions,
		// we handle that in the postfix handler
		token& identifier_token = previous();
		// only support type generics for now
		std::vector<type_handle> generic_args;
		if (ENABLE_GENERICS && match(TT_LESS)) {
			do {
				ast_ptr<type_node> gen_type = parse_type();
				if (gen_type) {
					generic_args.push_back(type_handle(gen_type));
				}
				else {
					ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
					return nullptr;
				}
			} while (match(TT_COMMA));
			if (!match(TT_GREATER)) {
				ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
				return nullptr;
			}
		}
		if (match(TT_LPAREN)) {
			std::vector<std::shared_ptr<expression>> args = parse_expression_list();
			if (!match(TT_RPAREN)) {
				ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
				return nullptr;
			}
			// identifier - function call
			else {
				auto func = make_ast<function_call>(identifier_token, identifier_token.value, args);
				func->generic_args = generic_args;
				expr = func;
			}
		}
		else {
			expr = make_ast<identifier_expression>(identifier_token, identifier_token.value);
		}
	}
	else if (match(TT_INTEGER_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, PRIMITIVE_I32, literal_token.value);
	}
	else if (match(TT_FLOAT_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, PRIMITIVE_FLOAT, literal_token.value);
	}
	else if (match(TT_CHAR_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, PRIMITIVE_CHAR, literal_token.value);
	}
	else if (match(TT_STRING_LITERAL)) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, PRIMITIVE_STRING, literal_token.value);
	}
	else if (match({ TT_TRUE, TT_FALSE })) {
		token& literal_token = previous();
		expr = make_ast<literal>(literal_token, PRIMITIVE_BOOL, literal_token.value);
	}
	else if (match(TT_THIS)) {
		expr = make_ast<this_expression>(previous());
	}
	else if (match(TT_LPAREN)) {
		// cast expression
		token& cast_token = previous();
		ast_ptr<type_node> cast_type = parse_type();
		if (cast_type) {
			if (!match(TT_RPAREN)) {
				ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
				return nullptr;
			}
			// parse the expression to cast
			auto cast_expr = parse_expression();
			if (!cast_expr) {
				ERROR_TOKEN(ERR_EXPRESSION_EXPECTED, peek());
				return nullptr;
			}
			expr = make_ast<cast_expression>(cast_token, cast_type, cast_expr);
		}
		// regular expression
		else {
			expr = parse_expression();
			if (!expr) {
				ERROR_TOKEN(ERR_EXPRESSION_EXPECTED, peek());
				return nullptr;
			}
			if (!match(TT_RPAREN)) {
				ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
				return nullptr;
			}
		}
	}
	else {
		ERROR_TOKEN(ERR_UNEXPECTED_TOKEN_PRIMARY_EXPR, peek());
		return nullptr;
	}

	// handle postfix operators
	while (true) {
		token& tok = peek();
		// member expressions
		if (tok.type == TT_DOT) {
			advance(); // consume '.'
			if (!match(TT_IDENTIFIER)) {
				ERROR_TOKEN(ERR_MEMBER_NAME_EXPECTED, peek());
				return nullptr;
			}
			token& member_token = previous();
			expr = make_ast<member_expression>(tok, expr, member_token.value);
		}
		// method call
		else if (tok.type == TT_LPAREN) {
			token& prev_tok = previous();
			advance(); // consume '('
			std::vector<std::shared_ptr<expression>> args = parse_expression_list();
			if (!match(TT_RPAREN)) {
				ERROR_TOKEN(ERR_RPAREN_EXPECTED, peek());
				return nullptr;
			}
			if (prev_tok.type == TT_IDENTIFIER) {
				expr = make_ast<function_call>(prev_tok, expr, prev_tok.value, args);
			}
			else {
				expr = make_ast<function_call>(tok, expr, args);
			}
		}
		// index expressions
		else if (tok.type == TT_LBRACKET) {
			advance();
			auto index_expr = parse_expression();
			if (!match(TT_RBRACKET)) {
				ERROR_TOKEN(ERR_RBRACKET_EXPECTED, peek());
				return nullptr;
			}
			expr = make_ast<index_expression>(tok, expr, index_expr);
		}
		// increment/decrement operators
		else if (tok.type == TT_INCREMENT || tok.type == TT_DECREMENT) {
			advance();
			expr = make_ast<unary_expression>(tok, tok.type, expr);
		}
		else {
			break;
		}
	}

	return expr;
}
ast_ptr<initializer_list> parser::parse_initializer_list() {
	token& init_token = peek();
	if (!match(TT_LBRACE)) {
		ERROR_TOKEN(ERR_LBRACE_EXPECTED, peek());
		return nullptr;
	}
	std::vector<std::shared_ptr<expression>> values = parse_expression_list(TT_RBRACE);
	if (!match(TT_RBRACE)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}
	return make_ast<initializer_list>(init_token, values);
}
ast_ptr<initializer_list> parser::parse_array_initializer() {
	token& init_token = peek();
	if (!match(TT_LBRACKET)) {
		ERROR_TOKEN(ERR_LBRACKET_EXPECTED, peek());
		return nullptr;
	}
	std::vector<std::shared_ptr<expression>> values = parse_expression_list(TT_RBRACKET);
	if (!match(TT_RBRACKET)) {
		ERROR_TOKEN(ERR_RBRACE_EXPECTED, peek());
		return nullptr;
	}
	auto list = make_ast<initializer_list>(init_token, values);
	list->is_array_initializer = true;
	return list;
}

ast_ptr<type_node> parser::parse_type() {
	// parse through modifiers, e.g. 'static'
	std::vector<data_type_modifier> type_mods;
	while (true) {
		auto modifier = to_type_modifier(previous().type);
		if (modifier == DTM_NONE) {
			break;
		}
		type_mods.push_back(modifier);
	}

	// match typename (custom or primitive)
	ast_ptr<type_node> type = nullptr;
	if (match_primitive() || match(TT_IDENTIFIER)) {
		token& tn_token = previous();
		type = make_ast<type_node>(tn_token, tn_token.value);
	}
	else {
		// type mods but no typename
		if (!type_mods.empty()) {
			ERROR_TOKEN(ERR_TYPENAME_EXPECTED, peek());
		}
		// no type or mods, so we can just return null
		return nullptr;
	}
	type->modifiers = type_mods;

	// parse through any pointer/array modifiers
	while (true) {
		if (match(TT_ASTERISK)) {
			type = make_ast<type_node>(type->position, type, type_node::kind::POINTER);
		}
		else if (match(TT_LBRACKET)) {
			ast_ptr<expression> array_size = nullptr;
			if (!match(TT_RBRACKET)) {
				array_size = parse_expression();
				if (!match(TT_RBRACKET)) {
					ERROR_TOKEN(ERR_RBRACKET_EXPECTED, peek());
					return nullptr;
				}
			}
			type = make_ast<type_node>(type->position, type, array_size);
		}
		else {
			break;
		}
	}

	return type;
}
std::vector<ast_ptr<expression>> parser::parse_expression_list(token_type end) {
	std::vector<ast_ptr<expression>> expressions;
	while (!is_at_end() && !check(end)) {
		auto expr = parse_expression();
		if (expr) {
			expressions.push_back(expr);
		}
		else {
			ERROR_TOKEN(ERR_EXPRESSION_EXPECTED, peek());
			return expressions;
		}
		if (match(TT_COMMA)) {
			continue; // continue to next expression
		}
	}
	return expressions;
}

std::vector<ast_ptr<variable_declaration>> parser::parse_parameter_list(token_type end) {
	std::vector<ast_ptr<variable_declaration>> parameters;
	if (check(end)) return parameters;
	do {
		auto param = parse_parameter();
		if (param) {
			parameters.push_back(param);
		}
		else {
			ERROR_TOKEN(ERR_PARAMETER_EXPECTED, peek());
			return parameters;
		}
	} while (!is_at_end() && !check(end) && match(TT_COMMA));
	return parameters;
}
std::vector<ast_ptr<generic_parameter>> parser::parse_generics() {
	std::vector<ast_ptr<generic_parameter>> generics;
	while (true) {
		if (match(TT_IDENTIFIER)) {
			token& id_token = previous();
			auto generic = make_ast<generic_parameter>(id_token, id_token.value);
			generics.push_back(generic);

			if (!match(TT_COMMA) && !check(TT_GREATER)) {
				ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
				return generics;
			}
		}
		else if (!match(TT_GREATER)) {
			ERROR_TOKEN(ERR_ANGLE_RIGHT_EXPECTED, peek());
			return generics;
		}
		else {
			return generics;
		}
	}
}

token& parser::peek() {
	return tokens[current];
}
token& parser::previous() {
	if (current == 0) {
		return tokens[0];
	}
	return tokens[current - 1];
}
token& parser::consume() {
	return tokens[current++];
}
bool parser::match_primitive() {
	if (match({ TT_I16, TT_I32, TT_I64, TT_FLOAT, TT_DOUBLE, TT_CHAR, TT_STRING, TT_BYTE, TT_BOOL, TT_VOID })) {
		return true;
	}
	return false;
}
bool parser::match_modifier() {
	return match({ TT_CONST, TT_STATIC, TT_EXPORT });
}
bool parser::match(std::initializer_list<token_type> types) {
	for (auto type : types) {
		if (match(type)) return true;
	}
	return false;
}
bool parser::match(token_type type) {
	if (is_at_end()) return false;
	if (peek().type != type) return false;
	advance();
	return true;
}
bool parser::check(token_type type) {
	if (is_at_end()) return false;
	return peek().type == type;
}
void parser::advance() {
	if (!is_at_end()) current++;
}
void parser::synchronize() {
	while (!is_at_end()) {
		if (previous().type == TT_SEMICOLON || previous().type == TT_RBRACE) return;
		if (peek().type == TT_RBRACE) return;

		switch (peek().type) {
		case TT_FUNC:
		case TT_CONST:
		case TT_LET:
		case TT_STRUCT:
		case TT_CLASS:
		case TT_INTERFACE:
		case TT_MODULE:
		case TT_IMPORT:
			return;
		}

		advance();
	}
}
bool parser::is_at_end() {
	return peek().type == TT_EOF;
}