#include <array>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "DirectoryComparer.hpp"

#include "DirectoryComparer.hpp"
#include "FileHashMapper.hpp"
#include <algorithm>  // For std::any_of
#include <array>
#include <iomanip>
#include <sstream>

void DirectoryComparer::process_directory(
    const fs::path& dir,
    std::unordered_map<std::string, std::string>& file_hashes,
    const std::vector<std::string>& exclude_folders) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        // Check if the file or directory matches an excluded folder pattern
        bool excluded = std::any_of(exclude_folders.begin(), exclude_folders.end(), [&](const std::string& folder) {
            return entry.path().string().find(folder) != std::string::npos;
        });

        if (!excluded && entry.is_regular_file()) {
            std::string relative_path = fs::relative(entry.path(), dir).string();
            file_hashes[relative_path] = FileHashMapper::compute_md5(entry.path());
        }
    }
}

void DirectoryComparer::compare_directories(const std::vector<fs::path>& directories, ComparisonMode mode, const std::vector<std::string>& exclude_folders) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> all_hashes;

    for (const auto& dir : directories) {
        std::unordered_map<std::string, std::string> file_hashes;
        process_directory(dir, file_hashes, exclude_folders);
        all_hashes[dir.string()] = file_hashes;
    }

    print_comparison(all_hashes, mode);
}

std::string DirectoryComparer::format_size(std::uintmax_t size) {
    constexpr std::array<const char*, 5> suffixes = {"B", "KB", "MB", "GB", "TB"};
    double readable_size = static_cast<double>(size);
    int suffix_index = 0;

    while (readable_size >= 1024 && suffix_index < suffixes.size() - 1) {
        readable_size /= 1024;
        ++suffix_index;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << readable_size << " " << suffixes[suffix_index];
    return oss.str();
}

void DirectoryComparer::print_comparison(
    const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& all_hashes,
    ComparisonMode mode) {
    for (const auto& [dir1, hashes1] : all_hashes) {
        for (const auto& [dir2, hashes2] : all_hashes) {
            if (dir1 >= dir2) continue;

            for (const auto& [file1, hash1] : hashes1) {
                auto it = hashes2.find(file1);
                if (it != hashes2.end() && hash1 == it->second) {
                    std::uintmax_t file_size = fs::file_size(file1);
                    std::string human_readable_size = DirectoryComparer::format_size(file_size);
                    std::cout << "Duplicate: Size: " << human_readable_size << "\n";
                    std::cout << file1 << "\n";
                    std::cout << it->first << "\n";
                }
            }
        }
    }
}

