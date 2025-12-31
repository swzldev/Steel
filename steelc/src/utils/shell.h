#pragma once

#include <string>

// shell
//
// helper class for executing os specific shell commands
// as well as handling output redirection and error handling

namespace shell {
	int exec(const std::string& command);
	std::string exec_piped(const std::string& command);
}