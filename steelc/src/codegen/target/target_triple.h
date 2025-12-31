#pragma once

#include <string>
#include <vector>

// target_triple
//
// this class wraps a target triple string

class target_triple {
public:
	explicit target_triple(const std::string& triple_str);

	// the underlying triple string
	std::string underlying_triple;

	// these fields may be empty if the target triple
	// does not specify them
	std::string arch;
	std::string vendor;
	std::string os;
	std::string abi;

private:
	void parse_triple(const std::string& triple_str);
};