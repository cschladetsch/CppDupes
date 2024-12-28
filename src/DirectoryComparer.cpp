#include "DirectoryComparer.hpp"
#include <openssl/evp.h>
#include <fstream>
#include <algorithm>
#include <iostream>

void DirectoryComparer::compare_directories(
    const std::vector<std::filesystem::path>& directories, 
    ComparisonMode mode, 
    const std::vector<std::string>& exclude_folders
) {
    // Validate input
    if (directories.size() < 2) {
        throw std::runtime_error("At least two directories are required for comparison");
    }

    // Concurrent hash computation
    std::vector<std::future<std::unordered_map<std::string, std::string>>> futures;
    for (const auto& dir : directories) {
        futures.push_back(std::async(std::launch::async, 
            [&dir, &exclude_folders]() {
                return process_directory(dir, exclude_folders);
            }
        ));
    }

    // Collect results
    std::vector<std::unordered_map<std::string, std::string>> directory_hashes;
    for (auto& future : futures) {
        directory_hashes.push_back(future.get());
    }

    // Compare hashes
    compare_hashes(directory_hashes, mode);
}

std::unordered_map<std::string, std::string> DirectoryComparer::process_directory(
    const std::filesystem::path& dir, 
    const std::vector<std::string>& exclude_folders
) {
    std::unordered_map<std::string, std::string> file_hashes;
    std::mutex hash_mutex;

    // Use parallel processing for directory iteration
    std::vector<std::thread> threads;
    std::atomic<size_t> file_count{0};

    for (const auto& entry : std::filesystem::recursive_directory_iterator(
        dir, 
        std::filesystem::directory_options::skip_permission_denied
    )) {
        // Skip excluded and non-regular files
        if (!entry.is_regular_file() || is_excluded(entry.path(), exclude_folders)) {
            continue;
        }

        // Process files in parallel
        threads.emplace_back([&, path = entry.path()]() {
            std::string hash = compute_file_hash(path);
            
            // Thread-safe insertion
            std::lock_guard<std::mutex> lock(hash_mutex);
            file_hashes[std::filesystem::relative(path, dir).string()] = hash;
        });

        // Limit concurrent threads
        if (threads.size() >= std::thread::hardware_concurrency()) {
            for (auto& t : threads) {
                t.join();
            }
            threads.clear();
        }
    }

    // Join any remaining threads
    for (auto& t : threads) {
        t.join();
    }

    return file_hashes;
}

bool DirectoryComparer::is_excluded(
    const std::filesystem::path& path, 
    const std::vector<std::string>& exclude_folders
) {
    for (const auto& component : path) {
        if (std::find(exclude_folders.begin(), exclude_folders.end(), component.string()) 
            != exclude_folders.end()) {
            return true;
        }
    }
    return false;
}

std::string DirectoryComparer::compute_file_hash(const std::filesystem::path& filepath) {
    constexpr size_t BUFFER_SIZE = 64 * 1024;  // 64KB buffer
    std::vector<char> buffer(BUFFER_SIZE);

    // Open file efficiently
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return {};
    }

    // Use EVP API for MD5
    EVP_MD_CTX* md5_context = EVP_MD_CTX_new();
    if (!md5_context) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;

    try {
        if (!EVP_DigestInit_ex(md5_context, EVP_md5(), nullptr)) {
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }

        while (file.read(buffer.data(), BUFFER_SIZE) || file.gcount() > 0) {
            if (!EVP_DigestUpdate(md5_context, buffer.data(), file.gcount())) {
                throw std::runtime_error("EVP_DigestUpdate failed");
            }
        }

        if (!EVP_DigestFinal_ex(md5_context, md_value, &md_len)) {
            throw std::runtime_error("EVP_DigestFinal_ex failed");
        }

    } catch (...) {
        EVP_MD_CTX_free(md5_context);
        throw;
    }

    EVP_MD_CTX_free(md5_context);

    // Convert MD5 to a hex string
    std::ostringstream oss;
    for (unsigned int i = 0; i < md_len; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md_value[i]);
    }

    return oss.str();
}

void DirectoryComparer::compare_hashes(
    const std::vector<std::unordered_map<std::string, std::string>>& directory_hashes,
    ComparisonMode mode
) {
    // Early exit if insufficient data
    if (directory_hashes.size() < 2) {
        return;
    }

    // Use unordered_map for fast lookups
    std::unordered_map<std::string, std::vector<std::pair<size_t, std::string>>> hash_index;

    // Build index of hashes
    for (size_t dir_idx = 0; dir_idx < directory_hashes.size(); ++dir_idx) {
        for (const auto& [filename, hash] : directory_hashes[dir_idx]) {
            hash_index[hash].emplace_back(dir_idx, filename);
        }
    }

    // Identify and process duplicate files
    for (const auto& [hash, file_list] : hash_index) {
        // Only care about hashes appearing in multiple directories
        if (file_list.size() > 1) {
            handle_duplicate_files(file_list, directory_hashes, mode);
        }
    }
}

void DirectoryComparer::handle_duplicate_files(
    const std::vector<std::pair<size_t, std::string>>& file_list,
    const std::vector<std::unordered_map<std::string, std::string>>& directory_hashes,
    ComparisonMode mode
) {
    // Basic implementation of duplicate file handling
    switch (mode) {
        case ComparisonMode::All:
        case ComparisonMode::OnlySame: {
            // Print files that are the same
            for (size_t i = 0; i < file_list.size(); ++i) {
                for (size_t j = i + 1; j < file_list.size(); ++j) {
                    std::cout << "Duplicate: " 
                              << file_list[i].second << " and " 
                              << file_list[j].second << '\n';
                }
            }
            break;
        }
        // Add other mode implementations as needed
        default:
            break;
    }
}
