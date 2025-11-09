#include "lexer.h"

#include "token_utils.h"

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
				ERROR(ERR_UNTERMINATED_COMMENT, { line, column });
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
	size_t col_start = column;
	std::string literal;
	bool seen_dot = false;

	token_type literal_type = TT_INTEGER_LITERAL;

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
				ERROR(ERR_INVALID_FLOAT_MULTIPLE_DECIMAL, { line, column });
				return true;
			}
			literal_type = TT_FLOAT_LITERAL;
			seen_dot = true;
			literal += c;
			index++;
			column++;

			// must have digits after the dot
			if (index >= src.length() || !isdigit(src[index])) {
				ERROR(ERR_INVALID_FLOAT_NO_DIGITS_AFTER_DECIMAL, { line, col_start });
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

	std::string literal;
	size_t i = index + 1;
	for (; src[i] != '\'' && i < src.length(); i++) {
		// do not allow line ends
		if (src[i] == '\n') {
			break;
		}
		literal += src[i];
	}

	// ensure char literal has an end
	if (src[i] != '\'') {
		ERROR(ERR_UNTERMINATED_CHAR_LITERAL, { line, column });
	}

	// parse result
	size_t true_length = literal.length();
	literal = parse_text_literal(literal);

	// ensure that the char literal is exactly one character long
	if (literal.length() != 1) { // 1 character
		ERROR(ERR_TOO_MANY_CHARS_IN_CHAR_LITERAL, { line, column });
	}

	column += true_length + 1;

	// add literal token
	add_token(literal, TT_CHAR_LITERAL);
	index += true_length + 1;

	return true;
}
bool lexer::check_for_string(std::string& src, size_t& index) {
	// check for beginning of string literal
	if (src[index] != '\"') {
		return false;
	}

	std::string literal;
	size_t i = index + 1;
	for (; src[i] != '\"' && i < src.length(); i++) {
		// do not allow line ends
		if (src[i] == '\n') {
			break;
		}
		literal += src[i];
	}

	// ensure string literal has an end
	if (src[i] != '\"') {
		ERROR(ERR_UNTERMINATED_STRING_LITERAL, { line, column });
	}

	// parse result
	size_t true_length = literal.length();
	literal = parse_text_literal(literal);

	column += true_length + 1;

	// add literal token
	add_token(literal, TT_STRING_LITERAL);
	index += true_length + 1;

	return true;
}

std::string lexer::parse_text_literal(std::string& literal) {
	std::string result;
	size_t idx = 0;

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
				ERROR(ERR_UNKNOWN_ESCAPE_SEQUENCE, { line, column + idx }, (std::string("\\") + nc).c_str());
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
	tokens.push_back({ value, type, line, column - value.length() });
}