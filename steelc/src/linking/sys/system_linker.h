#pragma once

#include <memory>
#include <string>
#include <vector>

// system_linker
//
// wraps a system linker (like ld, link.exe, etc) and provides
// an interface to interact with it easily

class system_linker {
public:
	static std::unique_ptr<system_linker> probe(const std::vector<std::string>& candidates);

	// note:
	// args should be escaped if necessary already
	int execute(const std::vector<std::string>& args) const;

	std::string executable_path;

private:
	system_linker(const std::string executable_path)
		: executable_path(executable_path) {
	}
};