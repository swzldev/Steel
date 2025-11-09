#include "debug_console.h"

#include <iostream>
#include <map>
#include <Windows.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

void debug_console::open() {
	is_open = true;

	// record the current console cursor position for backtracking
	auto console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console) {
		std::cout << "Failed to get console handle." << std::endl;
		return;
	}
	CONSOLE_SCREEN_BUFFER_INFO info{};
	GetConsoleScreenBufferInfo(console, &info);
	console_start_line = info.dwCursorPosition.Y;
}
void debug_console::close() {
	is_open = false;
}
void debug_console::refresh() {
	draw();
}
void debug_console::println(const std::string& ln) {
	std::cout << ln << std::endl;
	console_buffer.push_back(ln);
}
void debug_console::set_context(const source_file src, size_t line) {
	src_file = src;
	current_line = line;
}
debug_cmd debug_console::get_command() {
	std::cout << "> ";
	std::string input;
	while (true) {
		std::cin >> input;
		debug_cmd parsed = parse_command(input);
		if (parsed != DBG_CMD_UNKNOWN) {
			return parsed;
		}
		println("Unknown command.");
	}
}

void debug_console::draw() {
	if (!is_open) {
		return;
	}

	// clear console
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console) {
		std::cout << "Failed to get console handle." << std::endl;
		return;
	}
	CONSOLE_SCREEN_BUFFER_INFO info{};
	GetConsoleScreenBufferInfo(console, &info);
	COORD topLeft = { 0, static_cast<SHORT>(console_start_line) };
	DWORD written;
	FillConsoleOutputCharacterA(
		console,
		' ',
		info.dwSize.X * (info.dwSize.Y - console_start_line),
		topLeft,
		&written
	);
	// reset cursor position
	SetConsoleCursorPosition(console, topLeft);

	// print source context
	if (src_file.lines.empty()) {
		std::cout << "[No source loaded]" << std::endl;
		return;
	}

	// calculate lines to display
	int total_lines = static_cast<int>(src_file.lines.size());
	int half = SRC_CONTEXT_LINES / 2;
	int start = std::max(0, static_cast<int>(current_line) - half);
	int end = std::min(total_lines, start + SRC_CONTEXT_LINES);

	// adjust start if were near the end
	if (end - start < SRC_CONTEXT_LINES && start > 0) {
		start = std::max(0, end - SRC_CONTEXT_LINES);
	}

	// print context
	for (int i = start; i < end; ++i) {
		std::cout << (i + 1 < 10 ? " " : "") << (i + 1) << " ";

		if (i == static_cast<int>(current_line)) {
			std::cout << "-> ";
		}
		else {
			std::cout << "   ";
		}

		std::cout << src_file.lines[i] << std::endl;
	}

	std::cout << "\n";

	// print console buffer
	for (const auto& line : console_buffer) {
		std::cout << line << std::endl;
	}
}

debug_cmd debug_console::parse_command(std::string& cmd) {
	static const std::map<std::string, debug_cmd> command_map = {
		{ "resume", DBG_CMD_RESUME_EXECUTION },
		{ "step", DBG_CMD_STEP },
	};

	if (command_map.find(cmd) == command_map.end()) {
		return DBG_CMD_UNKNOWN;
	}
	return command_map.at(cmd);
}
