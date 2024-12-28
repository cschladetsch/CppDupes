#ifndef DIRECTORY_COMPARER_HPP
#define DIRECTORY_COMPARER_HPP

#include <filesystem>
#include <vector>
#include <string>
#include <atomic>

extern std::atomic<size_t> total_files;
extern std::atomic<size_t> total_bytes;

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
};

#endif // DIRECTORY_COMPARER_HPP

