#pragma once

#include <cstdarg>
#include <memory>
#include <vector>
#include <unordered_set>

#include <lexer/token.h>
#include <compiler/compilation_pass.h>
#include <ast/ast_fwd.h>
#include <symbolics/symbol_error.h>
#include <representations/types/types_fwd.h>

constexpr auto ENABLE_GENERICS = true;

class parser : public compilation_pass {
public:
	parser(std::shared_ptr<compilation_unit> unit, std::vector<token>& tokens)
		: unit(unit), tokens(tokens), compilation_pass(unit) {}

	void parse();
	ast_ptr parse_declaration();
	std::shared_ptr<function_declaration> parse_constructor_declaration(token& typename_token);
	std::shared_ptr<function_declaration> parse_function_declaration(bool is_constructor, bool is_override);
	std::shared_ptr<variable_declaration> parse_variable_declaration(bool is_const);
	std::shared_ptr<type_declaration> parse_type_declaration(token& kind_token);
	std::shared_ptr<enum_declaration> parse_enum_declaration();
	std::shared_ptr<variable_declaration> parse_parameter();
	ast_ptr parse_statement();
	std::shared_ptr<code_block> parse_block(bool is_body = true);
	std::shared_ptr<if_statement> parse_if_statement();
	std::shared_ptr<for_loop> parse_for_loop();
	std::shared_ptr<while_loop> parse_while_loop();
	std::shared_ptr<return_statement> parse_return_statement();
	std::shared_ptr<break_statement> parse_break_statement();
	ast_ptr parse_expression_statement();
	std::shared_ptr<expression> parse_expression();
	std::shared_ptr<expression> parse_binary_expression(int precedence = 0);
	std::shared_ptr<expression> parse_unary_expression();
	std::shared_ptr<expression> parse_primary_expression();
	std::shared_ptr<initializer_list> parse_initializer_list();
	std::shared_ptr<initializer_list> parse_array_initializer();

	// helper functions
	type_ptr parse_type();
	std::vector<std::shared_ptr<expression>> parse_expression_list(token_type end = TT_RPAREN);
	std::vector<std::shared_ptr<variable_declaration>> parse_parameter_list(token_type end = TT_RPAREN);
	std::vector<std::shared_ptr<generic_parameter>> parse_generics();

private:
	std::shared_ptr<compilation_unit> unit;
	std::vector<token>& tokens;
	size_t current = 0;

	token& peek();
	token& previous();
	token& consume();
	bool match_primitive();
	bool match_modifier();
	bool match(int count, ...);
	bool match(token_type type);
	bool check(token_type type);
	void advance();
	void synchronize();
	bool is_at_end();
};
