#pragma once

#include <string>
#include <filesystem>

namespace path_utils {
	std::filesystem::path normalize(const std::string& path);
	std::filesystem::path normalize(const std::filesystem::path& path);
}