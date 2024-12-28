#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <openssl/md5.h>
#include <iomanip>
#include <fstream>
#include <future>
#include <thread>
#include <mutex>

class FileHashMapper {
private:
    std::unordered_map<std::string, std::vector<std::filesystem::path>> hash_map;
    std::mutex map_mutex;

    static bool contains_git_path(const std::filesystem::path& path) {
        for (const auto& part : path) {
            if (part.string() == ".git") {
                return true;
            }
        }
        return false;
    }

    static std::string compute_file_md5(const std::filesystem::path& filepath) {
        constexpr size_t BUFFER_SIZE = 64 * 1024;
        std::vector<char> buffer(BUFFER_SIZE);

        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file) {
            return {};
        }

        auto file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file_size == 0) {
            return {};
        }

        MD5_CTX md5Context;
        MD5_Init(&md5Context);

        while (file.read(buffer.data(), BUFFER_SIZE)) {
            MD5_Update(&md5Context, buffer.data(), file.gcount());
        }

        if (file.gcount() > 0) {
            MD5_Update(&md5Context, buffer.data(), file.gcount());
        }

        unsigned char hash[MD5_DIGEST_LENGTH];
        MD5_Final(hash, &md5Context);

        char hex_hash[MD5_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
            snprintf(hex_hash + i * 2, 3, "%02x", hash[i]);
        }
        hex_hash[MD5_DIGEST_LENGTH * 2] = '\0';

        return hex_hash;
    }

    void process_directory(const std::filesystem::path& directory) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(
            directory, 
            std::filesystem::directory_options::skip_permission_denied
        )) {
            if (!entry.is_regular_file() || contains_git_path(entry.path())) continue;

            try {
                auto hash = compute_file_md5(entry.path());
                
                if (hash.empty()) continue;

                {
                    std::lock_guard<std::mutex> lock(map_mutex);
                    hash_map[hash].push_back(entry.path());
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
        }
    }

public:
    void find_duplicate_files(const std::filesystem::path& dir1, 
                               const std::filesystem::path& dir2) {
        std::vector<std::future<void>> futures;
        futures.push_back(std::async(std::launch::async, 
            [this, &dir1]() { process_directory(dir1); }));
        futures.push_back(std::async(std::launch::async, 
            [this, &dir2]() { process_directory(dir2); }));

        for (auto& future : futures) {
            future.wait();
        }

        for (const auto& [hash, paths] : hash_map) {
            if (paths.size() > 1) {
                for (size_t i = 0; i < paths.size(); ++i) {
                    for (size_t j = i + 1; j < paths.size(); ++j) {
                        std::cout << paths[i] << "\n" << paths[j] << "\n\n";
                    }
                }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <directory1> <directory2>\n";
        return 1;
    }

    try {
        std::filesystem::path dir1 = argv[1];
        std::filesystem::path dir2 = argv[2];

        if (!std::filesystem::exists(dir1) || !std::filesystem::is_directory(dir1) ||
            !std::filesystem::exists(dir2) || !std::filesystem::is_directory(dir2)) {
            std::cerr << "Error: Invalid directory\n";
            return 1;
        }

        FileHashMapper mapper;
        mapper.find_duplicate_files(dir1, dir2);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
