#include "DirectoryComparer.hpp"
#include <iostream>

void DirectoryComparer::compare_directories(const std::vector<fs::path>& directories, ComparisonMode mode) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> all_hashes;

    for (const auto& dir : directories) {
        std::unordered_map<std::string, std::string> file_hashes;
        process_directory(dir, file_hashes);
        all_hashes[dir.string()] = file_hashes;
    }

    print_comparison(all_hashes, mode);
}

void DirectoryComparer::process_directory(const fs::path& dir, std::unordered_map<std::string, std::string>& file_hashes) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string relative_path = fs::relative(entry.path(), dir).string();
            file_hashes[relative_path] = FileHashMapper::compute_md5(entry.path());
        }
    }
}

void DirectoryComparer::print_comparison(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& all_hashes, ComparisonMode mode) {
    for (const auto& [dir1, hashes1] : all_hashes) {
        for (const auto& [dir2, hashes2] : all_hashes) {
            if (dir1 >= dir2) continue;

            std::cout << "\nComparing " << dir1 << " and " << dir2 << ":\n";
            for (const auto& [file1, hash1] : hashes1) {
                auto it = hashes2.find(file1);
                if (it != hashes2.end()) {
                    if (hash1 == it->second) {
                        if (mode == ComparisonMode::All || mode == ComparisonMode::OnlySame) {
                            std::cout << "Match: " << file1 << " [Hash: " << hash1 << "]\n";
                        }
                    } else {
                        if (mode == ComparisonMode::All || mode == ComparisonMode::OnlyDifferent) {
                            std::cout << "Mismatch: " << file1 << "\n  Hash1: " << hash1 << "\n  Hash2: " << it->second << "\n";
                        }
                    }
                } else {
                    if (mode == ComparisonMode::All || mode == ComparisonMode::OnlyUnique) {
                        std::cout << "File only in " << dir1 << ": " << file1 << "\n";
                    }
                }
            }

            for (const auto& [file2, hash2] : hashes2) {
                if (hashes1.find(file2) == hashes1.end()) {
                    if (mode == ComparisonMode::All || mode == ComparisonMode::OnlyUnique) {
                        std::cout << "File only in " << dir2 << ": " << file2 << "\n";
                    }
                }
            }
        }
    }
}

