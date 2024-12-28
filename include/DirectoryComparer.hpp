#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

enum class ComparisonMode {
    All,
    OnlyDifferent,
    OnlySame,
    OnlyUnique
};

class DirectoryComparer {
public:
    // Remove the namespace declaration inside the class
    static void compare_directories(
        const std::vector<std::filesystem::path>& directories, 
        ComparisonMode mode, 
        const std::vector<std::string>& exclude_folders
    );

private:
    static void process_directory(
        const std::filesystem::path& dir, 
        std::unordered_map<std::string, std::string>& file_hashes, 
        const std::vector<std::string>& exclude_folders
    );

    static void print_comparison(
        const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& all_hashes,
        ComparisonMode mode
    );

    static std::string format_size(std::uintmax_t size);
};
