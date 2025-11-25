#pragma once

namespace console_colors {
	constexpr auto RESET = "\033[0m";
	constexpr auto BLACK = "\033[30m";
	constexpr auto RED = "\033[31m";
	constexpr auto GREEN = "\033[32m";
	constexpr auto YELLOW = "\033[33m";
	constexpr auto BLUE = "\033[34m";
	constexpr auto MAGENTA = "\033[35m";
	constexpr auto CYAN = "\033[36m";
	constexpr auto WHITE = "\033[37m";
	constexpr auto BOLD = "\033[1m";
	constexpr auto UNDERLINE = "\033[4m";
	constexpr auto REVERSED = "\033[7m";
	constexpr auto DIM = "\033[2m";

	void enable();
}