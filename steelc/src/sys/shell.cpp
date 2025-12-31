#include "shell.h"

#include <string>
#include <array>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>

#include <sys/host_defs.h>

int shell::exec(const std::string& command) {
	return std::system(command.c_str());
}
std::string shell::exec_piped(const std::string& command) {
	std::array<char, 4096> buffer{};
	std::string result;

#ifdef STEELC_PLATFORM_WINDOWS
	FILE* pipe = _popen(command.c_str(), "r");
#else
	FILE* pipe = popen(command.c_str(), "r");
#endif

	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
		result += buffer.data();
	}

#ifdef STEELC_PLATFORM_WINDOWS
	_pclose(pipe);
#else
	pclose(pipe);
#endif

	return result;
}
