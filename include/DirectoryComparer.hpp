#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <future>

enum class ComparisonMode {
    All,
    OnlyDifferent,
    OnlySame,
    OnlyUnique
};

class DirectoryComparer {
public:
    static void compare_directories(
        const std::vector<std::filesystem::path>& directories, 
        ComparisonMode mode, 
        const std::vector<std::string>& exclude_folders
    );

private:
    static std::unordered_map<std::string, std::string> process_directory(
        const std::filesystem::path& dir, 
        const std::vector<std::string>& exclude_folders
    );

    static bool is_excluded(
        const std::filesystem::path& path, 
        const std::vector<std::string>& exclude_folders
    );

    static std::string compute_file_hash(const std::filesystem::path& filepath);

    static void compare_hashes(
        const std::vector<std::unordered_map<std::string, std::string>>& directory_hashes,
        ComparisonMode mode
    );

    static void handle_duplicate_files(
        const std::vector<std::pair<size_t, std::string>>& file_list,
        const std::vector<std::unordered_map<std::string, std::string>>& directory_hashes,
        ComparisonMode mode
    );
};
