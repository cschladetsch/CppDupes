#include "DirectoryComparer.hpp"
#include <atomic>

// Define the external atomic variables
std::atomic<size_t> total_files(0);
std::atomic<size_t> total_bytes(0);

// Implement other functions
void DirectoryComparer::compare_directories(
    const std::vector<std::filesystem::path>& directories,
    ComparisonMode mode,
    const std::vector<std::string>& exclude_folders
) {
    for (const auto& dir : directories) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                ++total_files;
                total_bytes += entry.file_size();
            }
        }
    }
}

