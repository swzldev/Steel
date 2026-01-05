#include "target_triple.h"

target_triple::target_triple(const std::string& triple_str) {
	underlying_triple = triple_str;
	parse(triple_str);
}

void target_triple::parse(const std::string& triple_str) {
	unsigned index = 0;
	std::string token;
	for (char c : triple_str) {
		if (c == '-') {
			switch (index) {
			case 0:
				arch = token;
				break;
			case 1:
				vendor = token;
				break;
			case 2:
				os = token;
				break;
			case 3:
				abi = token;
				break;
			default:
				// ignore extra fields
				break;
			}

			token.clear();
			index++;
		}
		else {
			token += c;
		}
	}
}
