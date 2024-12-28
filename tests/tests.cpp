#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "FileHashMapper.hpp"

namespace fs = std::filesystem;

void setup_test_environment() {
    fs::create_directories("tests/empty_dir");
    fs::create_directories("tests/sample_dir");
    fs::create_directories("tests/duplicate_files_dir");
    fs::create_directories("tests/dir1");
    fs::create_directories("tests/dir2");
    fs::create_directories("tests/large_dir");
    fs::create_directories("tests/unique_hash_dir");

    std::ofstream("tests/sample_dir/sample.txt") << "Sample file";
    std::ofstream("tests/duplicate_files_dir/duplicate1.txt") << "Duplicate file";
    std::ofstream("tests/duplicate_files_dir/duplicate2.txt") << "Duplicate file";
    std::ofstream("tests/dir1/file1.txt") << "File 1 content";
    std::ofstream("tests/dir2/file2.txt") << "File 2 content";
    std::ofstream("tests/unique_hash_dir/file1.txt") << "Unique content 1";
    std::ofstream("tests/unique_hash_dir/file2.txt") << "Unique content 2";

    for (int i = 0; i < 100; ++i) {
        std::ofstream("tests/large_dir/file" + std::to_string(i) + ".txt") << "Large directory file " << i;
    }
}

void cleanup_test_environment() {
    fs::remove_all("tests/empty_dir");
    fs::remove_all("tests/sample_dir");
    fs::remove_all("tests/duplicate_files_dir");
    fs::remove_all("tests/dir1");
    fs::remove_all("tests/dir2");
    fs::remove_all("tests/large_dir");
    fs::remove_all("tests/unique_hash_dir");
}

class FileHashMapperTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        setup_test_environment();
    }

    void TearDown() override {
        cleanup_test_environment();
    }
};

::testing::Environment* const test_env = ::testing::AddGlobalTestEnvironment(new FileHashMapperTestEnvironment());

TEST(FileHashMapperTests, ProcessDirectoryWithNoFiles) {
    FileHashMapper mapper;
    mapper.process_directory("tests/empty_dir");
    EXPECT_EQ(mapper.get_file_count(), 0);
    EXPECT_EQ(mapper.get_total_size(), 0);
}

TEST(FileHashMapperTests, ProcessDirectoryWithFiles) {
    FileHashMapper mapper;
    mapper.process_directory("tests/sample_dir");
    EXPECT_EQ(mapper.get_file_count(), 1);
    EXPECT_GT(mapper.get_total_size(), 0);
}

TEST(FileHashMapperTests, MD5HashComputation) {
    std::string expected_md5 = "3e1b33daa65612e805821851958b5465"; // Correct MD5 for "Sample file"
    std::string actual_md5 = FileHashMapper::compute_md5("tests/sample_dir/sample.txt");
    EXPECT_EQ(actual_md5, expected_md5);
}

TEST(FileHashMapperTests, DuplicateFiles) {
    FileHashMapper mapper;
    mapper.process_directory("tests/duplicate_files_dir");
    EXPECT_EQ(mapper.get_file_count(), 2);
}

TEST(FileHashMapperTests, MissingDirectory) {
    FileHashMapper mapper;
    EXPECT_THROW(mapper.process_directory("tests/nonexistent_dir"), std::runtime_error);
}

TEST(FileHashMapperTests, CompareHashesBetweenDirectories) {
    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");
    EXPECT_NE(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST(FileHashMapperTests, LargeDirectory) {
    FileHashMapper mapper;
    mapper.process_directory("tests/large_dir");
    EXPECT_EQ(mapper.get_file_count(), 100);
}

TEST(FileHashMapperTests, FileSizeAccuracy) {
    // Create a dedicated test directory
    fs::create_directories("tests/size_accuracy_dir");

    // Create a file with exactly 1024 bytes
    std::ofstream file("tests/size_accuracy_dir/test_size.txt");
    file << std::string(1024, 'A'); // Write 1KB of 'A'
    file.close();

    FileHashMapper mapper;
    mapper.process_directory("tests/size_accuracy_dir");

    // Validate the total size
    EXPECT_EQ(mapper.get_total_size(), 1024); // Ensure size is exactly 1KB

    // Cleanup
    fs::remove_all("tests/size_accuracy_dir");
}

TEST(FileHashMapperTests, FileHashUniqueness) {
    FileHashMapper mapper;
    mapper.process_directory("tests/unique_hash_dir");
    auto hashes = mapper.get_file_hashes();
    EXPECT_EQ(hashes.size(), mapper.get_file_count());
}

TEST(FileHashMapperTests, EmptyFileHash) {
    std::ofstream file("tests/empty_file.txt");
    file.close();
    std::string hash = FileHashMapper::compute_md5("tests/empty_file.txt");
    EXPECT_EQ(hash, "d41d8cd98f00b204e9800998ecf8427e"); // MD5 of an empty file
    std::remove("tests/empty_file.txt");
}

