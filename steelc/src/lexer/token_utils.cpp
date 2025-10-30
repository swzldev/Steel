#include "token_utils.h"

#include <map>

bool is_identifier(token& tk) {
	return tk.type == TT_IDENTIFIER;
}
bool is_keyword(token& tk) {
	return tk.type >= TT_I16 && tk.type <= TT_VOID;
}
bool is_op(token& tk) {
	return tk.type >= TT_ASSIGN && tk.type <= TT_ACCESS;
}
bool is_eof(token& tk) {
	return tk.type == TT_EOF;
}
bool is_primitive(token& tk) {
	return tk.type >= TT_I16 && tk.type <= TT_VOID;
}

bool is_keyword(const std::string& tk) {
	return get_keyword(tk) != TT_UNKNOWN;
}
bool is_grammar(char tk) {
	return get_grammar(tk) != TT_UNKNOWN;
}
bool is_op(const std::string& tk) {
	return get_op(tk) != TT_UNKNOWN;
}

token_type get_keyword(const std::string& tk) {
	const auto& keywords = get_keywords();
	auto it = keywords.find(tk);
	if (it != keywords.end()) {
		return it->second;
	}
	return TT_UNKNOWN;
}
token_type get_grammar(char tk) {
	const auto& grammars = get_grammars();
	auto it = grammars.find(tk);
	if (it != grammars.end()) {
		return it->second;
	}
	return TT_UNKNOWN;
}
token_type get_op(const std::string& tk) {
	const auto& operators = get_operators();
	for (const auto& op : operators) {
		if (op.first == tk) {
			return op.second;
		}
	}
	return TT_UNKNOWN;
}

const std::map<std::string, token_type>& get_keywords() {
	static const std::map<std::string, token_type> keywords = {
		{"func", TT_FUNC},
		{"constructor", TT_CONSTRUCTOR},
		{"override", TT_OVERRIDE},
		{"let", TT_LET},
		{"const", TT_CONST},
		{"type", TT_TYPE},
		{"short", TT_I16},
		{"int", TT_I32},
		{"long", TT_I64},
		{"float", TT_FLOAT},
		{"double", TT_DOUBLE},
		{"char", TT_CHAR},
		{"string", TT_STRING},
		{"byte", TT_BYTE},
		{"bool", TT_BOOL},
		{"void", TT_VOID},
		{"if", TT_IF},
		{"else", TT_ELSE},
		{"while", TT_WHILE},
		{"for", TT_FOR},
		{"return", TT_RETURN},
		{"break", TT_BREAK},
		{"struct", TT_STRUCT},
		{"class", TT_CLASS},
		{"interface", TT_INTERFACE},
		{"enum", TT_ENUM},
		{"true", TT_TRUE},
		{"false", TT_FALSE},
		{"null", TT_NULL},
		{"this", TT_THIS},
		{"base", TT_BASE},
		{"module", TT_MODULE},
		{"import", TT_IMPORT},
		{"static", TT_STATIC},
		{"export", TT_EXPORT},
		{"and", TT_AND},
		{"or", TT_OR},
	};
	return keywords;
}
const std::vector<std::pair<std::string, token_type>>& get_operators() {
	static const std::vector<std::pair<std::string, token_type>> operators = {
		//{"and", TT_AND}, these are now keyword operators
		//{"or", TT_OR},
		{"++", TT_INCREMENT},
		{"--", TT_DECREMENT},
		{"==", TT_EQUAL},
		{"!=", TT_NOT_EQUAL},
		{"<=", TT_LESS_EQ},
		{">=", TT_GREATER_EQ},
		{"->", TT_ARROW},
		{"<", TT_LESS},
		{">", TT_GREATER},
		{"%", TT_MODULO},
		{"+", TT_ADD},
		{"-", TT_SUBTRACT},
		{"*", TT_MULTIPLY},
		{"/", TT_DIVIDE},
		{"=", TT_ASSIGN},
		{"!", TT_NOT},
		{".", TT_ACCESS},
	};
	return operators;
}
token_type get_op(const std::string& prgm, int i) {
	const auto& operators = get_operators();
	for (const auto& op : operators) {
		const std::string& op_str = op.first;
		if (prgm.compare(i, op_str.size(), op_str) == 0) {
			return op.second;
		}
	}
	return TT_UNKNOWN;
}
int op_len(token_type tk) {
	const auto& operators = get_operators();
	for (const auto& op : operators) {
		if (op.second == tk) {
			return op.first.size();
		}
	}
	return 0;
}

const std::map<char, token_type>& get_grammars() {
	static const std::map<char, token_type> grammars = {
		{';', TT_SEMICOLON},
		{':', TT_COLON},
		{',', TT_COMMA},
		{'(', TT_LPAREN},
		{')', TT_RPAREN},
		{'{', TT_LBRACE},
		{'}', TT_RBRACE},
		{'[', TT_LBRACKET},
		{']', TT_RBRACKET},
		{'^', TT_HAT},
		{'&', TT_AMPERSAND},
		{'#', TT_HASH},
		{'$', TT_DOLLAR},
	};
	return grammars;
}

int precedence_of(token_type tk) {
	static const std::map<token_type, int> precedence_map = {
		{TT_ACCESS, 40},
		{TT_INCREMENT, 30},
		{TT_DECREMENT, 30},
		{TT_NOT, 30},
		{TT_MULTIPLY, 20},
		{TT_DIVIDE, 20},
		{TT_MODULO, 20},
		{TT_ADD, 10},
		{TT_SUBTRACT, 10},
		{TT_LESS, 7},
		{TT_LESS_EQ, 7},
		{TT_GREATER, 7},
		{TT_GREATER_EQ, 7},
		{TT_EQUAL, 6},
		{TT_NOT_EQUAL, 6},
		{TT_AND, 3},
		{TT_OR, 2},
		{TT_ASSIGN, 1},
	};
	auto it = precedence_map.find(tk);
	if (it != precedence_map.end()) {
		return it->second;
	}
	return 0;
}

bool is_integer_literal(const std::string& value) {
	for (char c : value) {
		if (!isdigit(c)) return false;
	}
	return !value.empty();
}
bool is_float_literal(const std::string& value) {
	bool has_dot = false;
	for (char c : value) {
		if (c == '.') {
			if (has_dot) return false; // multiple dots
			has_dot = true;
		}
		else if (!isdigit(c)) {
			return false; // non-digit character
		}
	}
	return has_dot && !value.empty();
}
bool is_char_literal(const std::string& value) {
	return value.size() == 3 && value.front() == '\'' && value.back() == '\'';
}
bool is_string_literal(const std::string& value) {
	return value.size() >= 2 && value.front() == '"' && value.back() == '"';
}

std::string to_string(token_type tk) {
	switch (tk) {
	case TT_IDENTIFIER: return "identifier";

	case TT_INTEGER_LITERAL: return "integer literal";
	case TT_FLOAT_LITERAL: return "float literal";
	case TT_CHAR_LITERAL: return "character literal";
	case TT_STRING_LITERAL: return "string literal";

	case TT_SEMICOLON: return "semicolon";
	case TT_COLON: return "colon";
	case TT_COMMA: return "comma";
	case TT_LPAREN: return "left parenthesis";
	case TT_RPAREN: return "right parenthesis";
	case TT_LBRACE: return "left brace";
	case TT_RBRACE: return "right brace";
	case TT_LBRACKET: return "left bracket";
	case TT_RBRACKET: return "right bracket";

	case TT_I16: return "small";
	case TT_I32: return "int";
	case TT_I64: return "large";
	case TT_FLOAT: return "float";
	case TT_DOUBLE: return "double";
	case TT_CHAR: return "char";
	case TT_STRING: return "string";
	case TT_BYTE: return "byte";
	case TT_BOOL: return "bool";
	case TT_VOID: return "void";

	case TT_IF: return "if";
	case TT_ELSE: return "else";
	case TT_WHILE: return "while";
	case TT_FOR: return "for";
	case TT_RETURN: return "return";

	case TT_TRUE: return "true";
	case TT_FALSE: return "false";

	case TT_MODULE: return "module";
	case TT_EXPORT: return "export";
	case TT_IMPORT: return "import";

	case TT_ADD: return "+";
	case TT_SUBTRACT: return "-";
	case TT_MULTIPLY: return "*";
	case TT_DIVIDE: return "/";
	case TT_ASSIGN: return "=";
	case TT_EQUAL: return "==";
	case TT_NOT_EQUAL: return "!=";
	case TT_INCREMENT: return "++";
	case TT_DECREMENT: return "--";
	case TT_AND: return "and";
	case TT_OR: return "or";
	case TT_LESS: return "<";
	case TT_GREATER: return ">";
	case TT_LESS_EQ: return "<=";
	case TT_GREATER_EQ: return ">=";
	case TT_MODULO: return "%";
	case TT_NOT: return "not";
	case TT_ACCESS: return ".";

	case TT_EOF: return "end of file";

	default: return "unknown";
	}
}

std::string get_colored_representation(std::vector<token> tokens) {
	std::string representation;
	size_t current_line = 1;
	size_t current_col = 1;

	for (int i = 0; i < tokens.size() - 1 /* ignore EOF token */; i++) {
		token& tk = tokens[i];

		// add newlines if token is on a new line
		while (current_line < tk.pos.line) {
			representation += '\n';
			current_line++;
			current_col = 1;
		}

		// add spaces if token is further in the line
		while (current_col < tk.pos.column) {
			representation += ' ';
			current_col++;
		}

		// get colored string for token
		std::string colored = get_colored_representation(tk);
		representation += colored;
		current_col += tk.value.size();
	}

	return representation + "\033[0m";
}

std::string get_colored_representation(token& tk) {
	std::string color;
	switch (tk.type) {
	case TT_INTEGER_LITERAL:
	case TT_FLOAT_LITERAL:
		color = "\033[36m";
		break;
	case TT_CHAR_LITERAL:
	case TT_STRING_LITERAL:
		color = "\033[93m";
		break;
	case TT_IDENTIFIER:
		color = "\033[33m";
		break;
	case TT_I16:
	case TT_I32:
	case TT_I64:
	case TT_FLOAT:
	case TT_DOUBLE:
	case TT_CHAR:
	case TT_STRING:
	case TT_BYTE:
	case TT_BOOL:
	case TT_VOID:
		color = "\033[91m";
		break;
	case TT_IF:
	case TT_ELSE:
	case TT_WHILE:
	case TT_FOR:
	case TT_RETURN:
		color = "\033[91m";
		break;
	case TT_TRUE:
	case TT_FALSE:
	case TT_NULL:
	case TT_THIS:
	case TT_BASE:
		color = "\033[92m";
		break;
	case TT_STRUCT:
	case TT_CLASS:
	case TT_INTERFACE:
		color = "\033[95m";
		break;
	case TT_MODULE:
	case TT_EXPORT:
	case TT_IMPORT:
		color = "\033[91m";
		break;
	case TT_ADD:
	case TT_SUBTRACT:
	case TT_MULTIPLY:
	case TT_DIVIDE:
	case TT_ASSIGN:
	case TT_EQUAL:
	case TT_INCREMENT:
	case TT_DECREMENT:
	case TT_AND:
	case TT_OR:
	case TT_LESS:
	case TT_GREATER:
	case TT_LESS_EQ:
	case TT_GREATER_EQ:
	case TT_MODULO:
	case TT_NOT:
		color = "\033[37m";
		break;
	default:
		color = "\033[0m";
		break;
	}
	// dont bother to reset since all tokens have at least one color string
	return color + tk.value;
}
