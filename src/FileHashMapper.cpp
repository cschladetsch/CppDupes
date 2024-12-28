#include <openssl/evp.h>
#include <openssl/err.h>

#include "FileHashMapper.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

namespace fs = std::filesystem;

FileHashMapper::FileHashMapper() : file_count(0), total_size(0) {}

void FileHashMapper::process_directory(const fs::path& dir) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            // Store relative paths for consistent comparison
            std::string relative_path = fs::relative(entry.path(), dir).string();
            file_hashes[relative_path] = compute_md5(entry.path());
            //std::cout << "Processing file: " << entry.path() << " [Stored as: " << relative_path << "]\n";
            ++file_count;
            total_size += entry.file_size();
		}
        //} else {
            //std::cout << "Skipping non-regular file: " << entry.path() << "\n";
        //}
    }
}

size_t FileHashMapper::get_file_count() const {
    return file_count;
}

uintmax_t FileHashMapper::get_total_size() const {
    return total_size;
}

std::unordered_map<std::string, std::string> FileHashMapper::get_file_hashes() const {
    return file_hashes;
}
std::string FileHashMapper::compute_md5(const fs::path& file_path) {
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;

    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + file_path.string());
    }

    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    if (!md_ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    try {
        if (!EVP_DigestInit_ex(md_ctx, EVP_md5(), nullptr)) {
            throw std::runtime_error("EVP_DigestInit_ex failed");
        }

        char buffer[8192];
        while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
            if (!EVP_DigestUpdate(md_ctx, buffer, file.gcount())) {
                throw std::runtime_error("EVP_DigestUpdate failed");
            }
        }

        if (!EVP_DigestFinal_ex(md_ctx, md, &md_len)) {
            throw std::runtime_error("EVP_DigestFinal_ex failed");
        }
    } catch (...) {
        EVP_MD_CTX_free(md_ctx);
        throw;
    }

    EVP_MD_CTX_free(md_ctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < md_len; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md[i]);
    }
    return oss.str();
}
