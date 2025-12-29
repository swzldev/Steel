#include "system_linker.h"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>

#include <steelc/platform_constants.h>

namespace fs = std::filesystem;

std::unique_ptr<system_linker> system_linker::probe(const std::vector<std::string>& candidates) {
	const char* path = std::getenv("PATH");
	if (!path) {
		return nullptr;
	}

#if defined(STEELC_PLATFORM_WINDOWS)
	const char delim = ';';
#else
	const char delim = ':';
#endif

	std::stringstream ss(path);
	std::string dir;

	while (std::getline(ss, dir, delim)) {
		for (const std::string& c_filename : candidates) {
			fs::path candidate = fs::path(dir) / c_filename;
			if (fs::exists(candidate) && fs::is_regular_file(candidate)) {
				std::string linker_path = fs::canonical(candidate).string();
				return std::unique_ptr<system_linker>(new system_linker(linker_path));
			}
		}
	}

	return nullptr;
}

int system_linker::execute(const std::vector<std::string>& args) const {
	std::string args_str;
	for (const std::string& arg : args) {
		args_str += arg + " ";
	}

	std::string cmd = linker_path + " " + args_str;
	return std::system(cmd.c_str());
}
