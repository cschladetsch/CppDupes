#include "FileHashMapper.hpp"
#include <openssl/md5.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include "FileHashMapper.hpp"

FileHashMapper::FileHashMapper() : file_count(0), total_size(0) {}

void FileHashMapper::process_directory(const fs::path& dir) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            // Store relative paths for consistent comparison
            std::string relative_path = fs::relative(entry.path(), dir).string();
            file_hashes[relative_path] = compute_md5(entry.path());
            std::cout << "Processing file: " << entry.path() << " [Stored as: " << relative_path << "]\n";
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
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + file_path.string());
    }

    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5_ctx, buffer, file.gcount());
    }
    MD5_Update(&md5_ctx, buffer, file.gcount());

    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &md5_ctx);

    std::ostringstream oss;
    for (unsigned char byte : hash) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

