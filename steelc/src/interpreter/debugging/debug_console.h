#pragma once

#include <string>
#include <vector>

#include "../../stproj/source_file.h"

enum debug_cmd {
	DBG_CMD_UNKNOWN,
	DBG_CMD_RESUME_EXECUTION,
	DBG_CMD_STEP,
};

class debug_console {
public:
	debug_console() = default;

	void open();
	void close();
	void refresh();
	void println(const std::string& ln);
	void set_context(const source_file src, size_t line);
	debug_cmd get_command();

private:
	constexpr static int SRC_CONTEXT_LINES = 10;

	bool is_open = false;
	int console_start_line = 0;
	std::vector<std::string> console_buffer;
	source_file src_file;
	size_t current_line = 0;

	void draw();
	debug_cmd parse_command(std::string& cmd);
};