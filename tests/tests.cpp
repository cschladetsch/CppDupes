#include <gtest/gtest.h>
#include "FileHashMapper.hpp"
#include <filesystem>
#include <fstream>

class FileHashMapperTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        std::filesystem::create_directory("test_dir");
    }

    void TearDown() override {
        std::filesystem::remove_all("test_dir");
        std::filesystem::remove("large_file.txt");
    }

    void create_large_file(const std::string& filename, size_t lines) {
        std::ofstream file(filename);
        for (size_t i = 0; i < lines; ++i) {
            file << "Line " << i << ": Lorem ipsum dolor sit amet.\n";
        }
    }

    void create_test_files(size_t count, const std::string& dir = "test_dir") {
        for (size_t i = 0; i < count; ++i) {
            std::ofstream file(dir + "/file" + std::to_string(i) + ".txt");
            file << "Content for file " << i;
        }
    }

    FileHashMapper mapper;
};

TEST_F(FileHashMapperTestFixture, LargeFileHashTest) {
    create_large_file("large_file.txt", 100000);  // 100,000 lines
    ASSERT_NO_THROW(FileHashMapper::compute_md5("large_file.txt"));
}

TEST_F(FileHashMapperTestFixture, LargeDirectoryProcessing) {
    create_test_files(1000);  // Create 1,000 small files
    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 1000);
}

TEST_F(FileHashMapperTestFixture, NestedDirectoryProcessing) {
    std::filesystem::create_directory("test_dir/subdir");
    create_test_files(100, "test_dir");
    create_test_files(100, "test_dir/subdir");

    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 200);
}

TEST_F(FileHashMapperTestFixture, StressTestLargeFiles) {
    for (size_t i = 0; i < 10; ++i) {
        create_large_file("test_dir/large_file" + std::to_string(i) + ".txt", 50000);  // 50,000 lines each
    }
    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 10);
}

TEST_F(FileHashMapperTestFixture, DirectoryWithSpecialCharacters) {
    std::filesystem::create_directory("test_dir/special_çhår@ctérs");
    std::ofstream file("test_dir/special_çhår@ctérs/test.txt");
    file << "Special characters test";
    file.close();

    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 1);
}

TEST_F(FileHashMapperTestFixture, ConsistentHashAcrossLargeRuns) {
    create_large_file("large_file.txt", 100000);  // 100,000 lines
    std::string hash1 = FileHashMapper::compute_md5("large_file.txt");
    std::string hash2 = FileHashMapper::compute_md5("large_file.txt");
    ASSERT_EQ(hash1, hash2);
}

TEST_F(FileHashMapperTestFixture, DirectoryWithBinaryFiles) {
    std::ofstream binary_file("test_dir/binary_file.dat", std::ios::binary);
    for (size_t i = 0; i < 1000; ++i) {
        char data[1024] = { static_cast<char>(i % 256) };
        binary_file.write(data, sizeof(data));
    }
    binary_file.close();

    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 1);
}

TEST_F(FileHashMapperTestFixture, RecursiveHashVerification) {
    std::filesystem::create_directory("test_dir/subdir");
    create_test_files(10, "test_dir");
    create_test_files(10, "test_dir/subdir");

    mapper.process_directory("test_dir");
    auto hashes = mapper.get_file_hashes();

    for (const auto& [file, hash] : hashes) {
        ASSERT_FALSE(hash.empty());
    }
}

TEST_F(FileHashMapperTestFixture, PerformanceTestWithMixedFiles) {
    for (size_t i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            create_large_file("test_dir/large_file" + std::to_string(i) + ".txt", 10000);  // Large files
        } else {
            std::ofstream file("test_dir/file" + std::to_string(i) + ".txt");
            file << "Small content file";
        }
    }
    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 100);
}

TEST_F(FileHashMapperTestFixture, EmptyDirectoryTest) {
    mapper.process_directory("test_dir");
    ASSERT_EQ(mapper.get_file_count(), 0);
}
