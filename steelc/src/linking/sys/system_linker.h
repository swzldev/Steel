#pragma once

#include <memory>
#include <string>
#include <vector>

// system_linker
//
// mostly used to locate system linkers on PATH

class system_linker {
public:
	static std::unique_ptr<system_linker> probe(const std::vector<std::string>& candidates);

	std::string executable_path;

private:
	system_linker(const std::string executable_path)
		: executable_path(executable_path) {
	}
};