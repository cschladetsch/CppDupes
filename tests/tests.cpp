#include <gtest/gtest.h>
#include "FileHashMapper.hpp"
#include <filesystem>
#include <fstream>

class FileHashMapperTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup logic for all tests
        std::filesystem::create_directory("test_dir");
    }

    void TearDown() override {
        // Clean up after each test
        std::filesystem::remove_all("test_dir");
        std::filesystem::remove("test_valid.txt");
        std::filesystem::remove("test_empty.txt");
        std::filesystem::remove("test_hash.txt");
        std::filesystem::remove("large_file.txt");
    }

    FileHashMapper mapper;
};

TEST_F(FileHashMapperTestFixture, ComputeMd5ValidFile) {
    std::ofstream file("test_valid.txt");
    file << "test content";
    file.close();

    std::string hash = FileHashMapper::compute_md5("test_valid.txt");
    ASSERT_FALSE(hash.empty());
}

TEST_F(FileHashMapperTestFixture, ComputeMd5EmptyFile) {
    std::ofstream file("test_empty.txt");
    file.close();

    std::string hash = FileHashMapper::compute_md5("test_empty.txt");
    ASSERT_FALSE(hash.empty());
}

TEST_F(FileHashMapperTestFixture, ComputeMd5NonexistentFile) {
    ASSERT_THROW(FileHashMapper::compute_md5("nonexistent.txt"), std::exception);
}

TEST_F(FileHashMapperTestFixture, ProcessDirectorySingleFile) {
    std::ofstream file("test_dir/file1.txt");
    file << "content";
    file.close();

    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 1);
}

TEST_F(FileHashMapperTestFixture, ProcessDirectoryMultipleFiles) {
    std::ofstream file1("test_dir/file1.txt");
    file1 << "content1";
    file1.close();

    std::ofstream file2("test_dir/file2.txt");
    file2 << "content2";
    file2.close();

    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 2);
}

TEST_F(FileHashMapperTestFixture, ProcessDirectoryNoFiles) {
    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 0);
}

TEST_F(FileHashMapperTestFixture, ComputeMd5ConsistentHash) {
    std::ofstream file("test_hash.txt");
    file << "consistent content";
    file.close();

    std::string hash1 = FileHashMapper::compute_md5("test_hash.txt");
    std::string hash2 = FileHashMapper::compute_md5("test_hash.txt");

    ASSERT_EQ(hash1, hash2);
}

TEST_F(FileHashMapperTestFixture, GetFileHashes) {
    std::ofstream file("test_dir/file1.txt");
    file << "content";
    file.close();

    mapper.process_directory("test_dir");
    auto hashes = mapper.get_file_hashes();
    ASSERT_EQ(hashes.size(), 1);
}

TEST_F(FileHashMapperTestFixture, ComputeMd5Performance) {
    std::ofstream file("large_file.txt");
    for (int i = 0; i < 100000; ++i) {
        file << "large_content\n";
    }
    file.close();

    ASSERT_NO_THROW(FileHashMapper::compute_md5("large_file.txt"));
}

