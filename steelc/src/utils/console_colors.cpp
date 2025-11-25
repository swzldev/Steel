#include "console_colors.h"

#ifdef _WIN32
#include <Windows.h>
#endif

void console_colors::enable() {
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole != INVALID_HANDLE_VALUE) {
		DWORD mode;
		GetConsoleMode(hConsole, &mode);
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hConsole, mode);
	}
#endif
}