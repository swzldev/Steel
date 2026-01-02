#include "lexer.h"

#include <lexer/token_utils.h>

std::vector<token> lexer::tokenize() {
	// seperate source into tokens
	std::string word;
	for (size_t i = 0; i < source.length(); i++) {
		char c = source[i];

		if (i + 1 < source.length() && source[i] == '/' && source[i + 1] == '/') {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}

			// single line comment, skip to end of line
			i += 2;
			column += 2;
			while (i < source.length() && source[i] != '\n') {
				i++;
				column++;
			}
			i--; // step back so the for loop can increment
			continue;
		}
		else if (i + 1 < source.length() && source[i] == '/' && source[i + 1] == '*') {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}

			position start = { line, column };

			// multi line comment, skip to closing */
			i += 2;
			column += 2;
			while (i + 1 < source.length() && !(source[i] == '*' && source[i + 1] == '/')) {
				if (source[i] == '\n') {
					line++;
					column = 1;
				}
				else {
					column++;
				}
				i++;
			}
			if (i + 1 >= source.length()) {
				// unterminated comment
				ERROR(ERR_UNTERMINATED_COMMENT, code_span(start, { line, column }));
				return tokens;
			}
			// skip closing */
			i += 1;
			column += 2;
			continue;
		}

		// skip whitespace
		if (c == ' ') {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}
			column++;
			continue;
		}

		// skip tabs
		if (c == '\t') {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}
			column += 4;
			continue;
		}

		// skip newlines
		if (c == '\n') {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}
			line++;
			column = 1;
			continue;
		}

		// check for character based literals
		if (check_for_char(source, i)) {
			word.clear();
			continue;
		}
		else if (check_for_string(source, i)) {
			word.clear();
			continue;
		}

		// check for number literals
		if (isdigit(c)) {
			if (word.empty()) {
				if (check_for_number(source, i)) {
					continue;
				}
			}
			else {
				word += c;
				column++;
			}
			continue;
		}

		// check for grammars
		if (is_grammar(c)) {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}
			// add grammar token
			token_type grammar = get_grammar(c);
			column++;
			add_token(std::string(1, c), grammar);
			continue;
		}

		// check for operators
		token_type op = get_op(source, i);
		if (op != TT_UNKNOWN) {
			if (!word.empty()) {
				add_token(word);
				word.clear();
			}
			// add operator token
			int len = op_len(op);
			column += len;
			add_token(source.substr(i, len), op);
			i += (size_t)len - 1;
			continue;
		}

		// add to word
		word += c;
		column++;
	}
	if (!word.empty()) {
		add_token(word);
		word.clear();
	}

	// add eof token
	add_token("", TT_EOF);

	return tokens;
}

bool lexer::check_for_number(std::string& src, size_t& index) {
	std::string literal;
	bool seen_dot = false;

	token_type literal_type = TT_INTEGER_LITERAL;

	position start = { line, column };

	while (index < src.length()) {
		char c = src[index];

		if (isdigit(c)) {
			literal += c;
			index++;
			column++;
		}
		else if (c == '.') {
			if (seen_dot) {
				// error: multiple dots in number
				code_span span = code_span(start, { line, column });
				ERROR(ERR_INVALID_FLOAT_MULTIPLE_DECIMAL, span);
				return true;
			}
			literal_type = TT_FLOAT_LITERAL;
			seen_dot = true;
			literal += c;
			index++;
			column++;

			// must have digits after the dot
			if (index >= src.length() || !isdigit(src[index])) {
				code_span span = code_span(start, { line, column });
				ERROR(ERR_INVALID_FLOAT_NO_DIGITS_AFTER_DECIMAL, span);
				return true;
			}
		}
		else {
			break;
		}
	}
	
	add_token(literal, literal_type);

	index--;
	return true;
}
bool lexer::check_for_char(std::string& src, size_t& index) {
	// check for beginning of char literal
	if (src[index] != '\'') {
		return false;
	}

	position start = { line, column };

	std::string literal;
	size_t i = index + 1;
	for (; src[i] != '\'' && i < src.length(); i++) {
		// do not allow line ends
		if (src[i] == '\n') {
			break;
		}
		literal += src[i];
	}

	size_t true_length = literal.length();

	// ensure char literal has an end
	if (src[i] != '\'') {
		code_span span = code_span(start, { line, column + true_length });
		ERROR(ERR_UNTERMINATED_CHAR_LITERAL, span);
	}

	// parse result
	literal = parse_text_literal(literal);

	// ensure that the char literal is exactly one character long
	if (literal.length() != 1) { // 1 character
		code_span span = code_span(start, { line, column });
		ERROR(ERR_TOO_MANY_CHARS_IN_CHAR_LITERAL, span);
	}
	
	// see note in check_for_string about line and column handling

	// add literal token
	add_token(literal, TT_CHAR_LITERAL, start, { line, column + i - index });
	column += true_length + 2; // both quotes
	index += true_length + 1; // for loop will increment one more (closing quote)

	return true;
}
bool lexer::check_for_string(std::string& src, size_t& index) {
	// check for beginning of string literal
	if (src[index] != '\"') {
		return false;
	}

	position start = { line, column };

	std::string literal;
	size_t i = index + 1;
	for (; src[i] != '\"' && i < src.length(); i++) {
		// do not allow line ends
		if (src[i] == '\n') {
			break;
		}
		literal += src[i];
	}

	size_t true_length = literal.length();

	// ensure string literal has an end
	if (src[i] != '\"') {
		position end = { line, column + true_length };
		code_span span = code_span(start, end);
		ERROR(ERR_UNTERMINATED_STRING_LITERAL, span);
	}

	// parse result
	literal = parse_text_literal(literal);

	// important notes:
	// we need to provide the line and column manually,
	// this is because string and char literals' sizes may
	// differ from that in the source code, due to the removal
	// of escape sequences, quotes, etc.
	// therefore, we cannot rely on subtracting the literal length
	// from the current column to get the starting column.
	// in this case to do so we just use the current column and
	// increment it after adding the token (which usually wouldnt
	// be possible)

	// add literal token
	add_token(literal, TT_STRING_LITERAL, start, { line, column + i - index });
	column += true_length + 2; // both quotes
	index += true_length + 1; // for loop will increment one more (closing quote)

	return true;
}

std::string lexer::parse_text_literal(std::string& literal) {
	std::string result;
	size_t idx = 0;


	// always safe next getter
	auto next = [&]() -> char {
		if (idx + 1 < literal.length()) {
			return literal[idx + 1];
		}
		return '\0';
	};

	for (; idx < literal.length(); idx++) {
		char c = literal[idx];
		// catch all escape sequences here
		if (c == '\\') {
			// store start of escape sequence
			position start = { line, column + idx + 1 };

			char nc = next();
			switch (nc) {
			case 'n':
				result += '\n';
				break;
			case 't':
				result += '\t';
				break;
			case 'r':
				result += '\r';
				break;
			case '\\':
				result += '\\';
				break;
			case '\"':
				result += '\"';
				break;
			case '\'':
				result += '\'';
				break;
			default:
				code_span span = code_span(start, { line, column + idx + 2 });
				ERROR(ERR_UNKNOWN_ESCAPE_SEQUENCE, span, (std::string("\\") + nc).c_str());
				break;
			}
			idx++; // skip next char
		}
		else {
			result += c;
		}
	}
	return result;
}

void lexer::add_token(const std::string& value) {
	// early return for empty strings
	if (value.empty()) return;

	// check if the token is a keyword
	if (is_keyword(value)) {
		add_token(value, get_keyword(value));
	}

	// add token
	else {
		add_token(value, TT_IDENTIFIER);
	}
}
void lexer::add_token(const std::string& value, token_type type) {
	position start = { line, column - value.length() };
	position end = { line, column - 1 }; // end is the last character not the one after

	tokens.push_back({ value, type, code_span(start, end) });
}
void lexer::add_token(const std::string& value, token_type type, position start, position end) {
	tokens.push_back({ value, type, code_span(start, end) });
}
