#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "FileHashMapper.hpp"

TEST(FileHashMapperTests, ComputeMd5ValidFile) {
    std::ofstream file("test_valid.txt");
    file << "test content";
    file.close();

    std::string hash = FileHashMapper::compute_md5("test_valid.txt");
    std::filesystem::remove("test_valid.txt");

    ASSERT_FALSE(hash.empty());
}

TEST(FileHashMapperTests, ComputeMd5EmptyFile) {
    std::ofstream file("test_empty.txt");
    file.close();

    std::string hash = FileHashMapper::compute_md5("test_empty.txt");
    std::filesystem::remove("test_empty.txt");

    ASSERT_FALSE(hash.empty());
}

TEST(FileHashMapperTests, ComputeMd5NonexistentFile) {
    ASSERT_THROW(FileHashMapper::compute_md5("nonexistent.txt"), std::exception);
}

TEST(FileHashMapperTests, ProcessDirectorySingleFile) {
    std::filesystem::create_directory("test_dir");
    std::ofstream file("test_dir/file1.txt");
    file << "content";
    file.close();

    FileHashMapper mapper;
    mapper.process_directory("test_dir");

    ASSERT_EQ(mapper.get_file_count(), 1);
    std::filesystem::remove_all("test_dir");
}

TEST(FileHashMapperTests, ProcessDirectoryMultipleFiles) {
    std::filesystem::create_directory("test_dir");
    std::ofstream file1("test_dir/file1.txt");
    file1 << "content1";
    file1.close();

    std::ofstream file2("test_dir/file2.txt");
    file2 << "content2";
    file2.close();

    FileHashMapper mapper;
    mapper.process_directory("test_dir");

    ASSERT_EQ(mapper.get_file_count(), 2);
    std::filesystem::remove_all("test_dir");
}

TEST(FileHashMapperTests, ProcessDirectoryNoFiles) {
    std::filesystem::create_directory("test_dir");

    FileHashMapper mapper;
    mapper.process_directory("test_dir");

    ASSERT_EQ(mapper.get_file_count(), 0);
    std::filesystem::remove_all("test_dir");
}

TEST(FileHashMapperTests, ComputeMd5ConsistentHash) {
    std::ofstream file("test_hash.txt");
    file << "consistent content";
    file.close();

    std::string hash1 = FileHashMapper::compute_md5("test_hash.txt");
    std::string hash2 = FileHashMapper::compute_md5("test_hash.txt");

    ASSERT_EQ(hash1, hash2);
    std::filesystem::remove("test_hash.txt");
}

TEST(FileHashMapperTests, ProcessDirectoryIgnoresSubdirectories) {
    std::filesystem::create_directory("test_dir");
    std::filesystem::create_directory("test_dir/subdir");
    std::ofstream file("test_dir/file1.txt");
    file << "content";
    file.close();

    FileHashMapper mapper;
    mapper.process_directory("test_dir");

    ASSERT_EQ(mapper.get_file_count(), 1);
    std::filesystem::remove_all("test_dir");
}

TEST(FileHashMapperTests, GetFileHashes) {
    std::filesystem::create_directory("test_dir");
    std::ofstream file("test_dir/file1.txt");
    file << "content";
    file.close();

    FileHashMapper mapper;
    mapper.process_directory("test_dir");

    auto hashes = mapper.get_file_hashes();
    ASSERT_EQ(hashes.size(), 1);

    std::filesystem::remove_all("test_dir");
}

TEST(FileHashMapperTests, ComputeMd5Performance) {
    std::ofstream file("large_file.txt");
    for (int i = 0; i < 100000; ++i) {
        file << "large_content\n";
    }
    file.close();

    ASSERT_NO_THROW(FileHashMapper::compute_md5("large_file.txt"));
    std::filesystem::remove("large_file.txt");
}
