#pragma once

#include <string>
#include <memory>

#include <representations/types/types_fwd.h>
#include <mir/mir_fwd.h>

// name mangler
// the name mangler in steel follows the itanium c++ abi mangling scheme
// this is to ensure compatibility with other languages and tools that are built
// around it

class name_mangler {
public:
	std::string mangle_function(const mir_function& fn_mir);

private:
	std::string mangle_text(const std::string& text);
	std::string mangle_type(const mir_type& ty);
};