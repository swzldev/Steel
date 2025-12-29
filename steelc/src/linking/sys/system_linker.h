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

	int execute(const std::vector<std::string>& args) const;

private:
	system_linker(const std::string linker_path)
		: linker_path(linker_path) {
	}

	std::string linker_path;
};