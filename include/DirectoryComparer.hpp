#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>
#include "FileHashMapper.hpp"

namespace fs = std::filesystem;

enum class ComparisonMode {
    All,
    OnlyDifferent,
    OnlySame,
    OnlyUnique
};

class DirectoryComparer {
public:
    static void compare_directories(const std::vector<fs::path>& directories, ComparisonMode mode, const std::vector<std::string>& exclude_folders);

private:
    static void process_directory(const fs::path& dir, std::unordered_map<std::string, std::string>& file_hashes, const std::vector<std::string>& exclude_folders);
    static void print_comparison(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& all_hashes, ComparisonMode mode);
};

