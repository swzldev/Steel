#include "error_printer.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>

#include "error.h"
#include "../parser/ast/compilation_unit.h"
#include "../utils/console_colors.h"

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
		<< console_colors::CYAN << error.pos.line << console_colors::RESET << ":"
		<< console_colors::CYAN << error.pos.column << console_colors::RESET << ": "
		<< console_colors::BOLD << error.info.message << console_colors::RESET << "\n";

	if (src && error.pos.line > 0 && error.pos.line <= src->size()) {
		int start_line = std::max(1, (int)error.pos.line - 2);
		int end_line = error.pos.line;
		int line_num_width = std::to_string(src->size()).size();

		for (int i = start_line; i <= end_line; ++i) {
			const auto& code_line = (*src)[(size_t)i - 1];
			std::cerr << console_colors::DIM << std::setw(line_num_width) << i << " | " << console_colors::RESET << code_line << "\n";
			if (i == error.pos.line) {
				std::cerr << std::setw(line_num_width) << " " << " | "
					<< std::string(error.pos.column - 1, ' ')
					<< console_colors::BOLD << console_colors::RED << "^\033[0m" << console_colors::RESET << " HERE\n";
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