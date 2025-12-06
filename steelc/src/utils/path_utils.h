#pragma once

#include <string>
#include <filesystem>

namespace path_utils {
	std::filesystem::path normalize_path(const std::string& path);
	std::filesystem::path normalize_path(const std::filesystem::path& path);
}