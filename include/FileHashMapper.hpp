#ifndef FILE_HASH_MAPPER_HPP
#define FILE_HASH_MAPPER_HPP

#include <string>
#include <filesystem>
#include <unordered_map>
#include <atomic>

class FileHashMapper {
public:
    FileHashMapper();
    void process_directory(const std::filesystem::path& dir);
    size_t get_file_count() const;
    uintmax_t get_total_size() const;
    std::unordered_map<std::string, std::string> get_file_hashes() const;
    static std::string compute_md5(const std::filesystem::path& file_path);

private:
    std::unordered_map<std::string, std::string> file_hashes;
    std::atomic<size_t> file_count;
    std::atomic<uintmax_t> total_size;

};

#endif // FILE_HASH_MAPPER_HPP

