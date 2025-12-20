#include "path_utils.h"

#include <filesystem>

std::filesystem::path path_utils::normalize(const std::string& path) {
	std::filesystem::path p(path);

	if (p.is_relative()) {
		p = std::filesystem::absolute(p);
	}

	p = p.lexically_normal();
	p.make_preferred();

	return p;
}
std::filesystem::path path_utils::normalize(const std::filesystem::path& path) {
	return normalize(path.string());
}
