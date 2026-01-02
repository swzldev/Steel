#include "error_printer.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

#include <error/error.h>
#include <ast/compilation_unit.h>
#include <utils/console_colors.h>

void error_printer::print_errors(const std::vector<error>& errors) {
	for (const auto& err : errors) {
		print_error(err);
	}
}
void error_printer::print_error(const error& error) {
	std::string file_name = "<unknown file>";
	const std::vector<std::string>* src = nullptr;

	if (error.unit && error.unit->source_file) {
		file_name = error.unit->source_file->name();
		src = &error.unit->source_file->lines;
	}

	// ERROR <code>:
	std::cerr << console_colors::RED << console_colors::BOLD
		<< "ERROR " << error.info.code << ":" << console_colors::RESET << "\n";

	// <file>:<line>:<col>: <message>
	std::cerr << console_colors::MAGENTA << file_name << console_colors::RESET << ":"
		<< console_colors::CYAN << error.span.start.line << console_colors::RESET << ":"
		<< console_colors::CYAN << error.span.start.column << console_colors::RESET << ": "
		<< console_colors::BOLD << error.info.message << console_colors::RESET << "\n";

	if (src && error.span.start.line > 0 && error.span.start.line <= src->size()) {
		int start_line = std::max(1, (int)error.span.start.line - 2);
		int end_line = error.span.start.line;
		int line_num_width = std::to_string(src->size()).size();

		for (int i = start_line; i <= end_line; ++i) {
			const auto& code_line = (*src)[(size_t)i - 1];
			std::cerr << console_colors::DIM << std::setw(line_num_width) << i << " | " << console_colors::RESET << code_line << "\n";
			if (i == error.span.start.line) {
				// calculate the length of the error span on this line
				int span_length = 1; // default to at least 1 character
				if (error.span.start.line == error.span.end.line) {
					// error is on a single line, calculate actual span length
					span_length = std::max(1, (int)(error.span.end.column - error.span.start.column));
				}
				
				// leading spaces
				std::cerr << std::setw(line_num_width) << " " << " | "
					<< std::string(std::max(0, (int)error.span.start.column - 2), ' ');
				
				// indicator
				std::cerr << console_colors::BOLD << console_colors::RED;
				
				// use '^' for the first character and '~' for the rest
				std::cerr << "^";
				if (span_length > 0) {
					std::cerr << std::string(span_length, '~');
				}
				std::cerr << console_colors::RESET << " HERE\n";
			}
		}
	}
	else if (src) {
		std::cerr << console_colors::BOLD << console_colors::RED << "Line number out of range." << console_colors::RESET << "\n";
	}
	else {
		std::cerr << console_colors::BOLD << console_colors::RED << "Source file not found." << console_colors::RESET << "\n";
	}

	std::cerr << std::endl;
	for (const auto& advice : error.advices) {
		std::cerr << console_colors::BOLD << console_colors::BLUE << "Advice " << advice.info.code << ": " << console_colors::RESET
			<< advice.info.message << "." << console_colors::RESET << "\n\n";
	}
	std::cerr << std::endl;
}
