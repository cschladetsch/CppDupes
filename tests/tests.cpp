#include <gtest/gtest.h>
#include "DirectoryComparer.hpp"
#include "FileHashMapper.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

// Test Fixture for Automated Cleanup
class FileHashMapperTestFixture : public ::testing::Test {
protected:
    std::vector<std::string> directories_to_clean;

    void SetUp() override {
        directories_to_clean.clear();
    }

    void TearDown() override {
        for (const auto& dir : directories_to_clean) {
            if (fs::exists(dir)) {
                fs::remove_all(dir);
            }
        }
    }

    void create_test_directory(const std::string& path) {
        fs::create_directories(path);
        directories_to_clean.push_back(path);
    }

    void create_test_file(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }
};

// Tests
TEST_F(FileHashMapperTestFixture, ProcessEmptyDirectory) {
    create_test_directory("tests/empty_dir");

    FileHashMapper mapper;
    mapper.process_directory("tests/empty_dir");
    EXPECT_EQ(mapper.get_file_count(), 0);
    EXPECT_EQ(mapper.get_total_size(), 0);
}

TEST_F(FileHashMapperTestFixture, ProcessNonEmptyDirectory) {
    create_test_directory("tests/sample_dir");
    create_test_file("tests/sample_dir/file1.txt", "Sample content");

    ASSERT_TRUE(fs::exists("tests/sample_dir/file1.txt")) << "Test file was not created.";

    FileHashMapper mapper;
    mapper.process_directory("tests/sample_dir");
    EXPECT_EQ(mapper.get_file_count(), 1);
    EXPECT_GT(mapper.get_total_size(), 0);
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithSameContent) {
    create_test_directory("tests/dir1");
    create_test_directory("tests/dir2");
    create_test_file("tests/dir1/file.txt", "Same content");
    create_test_file("tests/dir2/file.txt", "Same content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");

    EXPECT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST_F(FileHashMapperTestFixture, CompareNestedDirectories) {
    create_test_directory("tests/nested_dir1/subdir");
    create_test_directory("tests/nested_dir2/subdir");
    create_test_file("tests/nested_dir1/subdir/file.txt", "Nested content");
    create_test_file("tests/nested_dir2/subdir/file.txt", "Nested content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/nested_dir1");
    mapper2.process_directory("tests/nested_dir2");

    EXPECT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST_F(FileHashMapperTestFixture, HandleHiddenFiles) {
    create_test_directory("tests/hidden_files");
    create_test_file("tests/hidden_files/.hidden_file", "Hidden content");

    ASSERT_TRUE(fs::exists("tests/hidden_files/.hidden_file")) << "Hidden file was not created.";

    FileHashMapper mapper;
    mapper.process_directory("tests/hidden_files");

    EXPECT_EQ(mapper.get_file_count(), 1);
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithAdditionalFiles) {
    create_test_directory("tests/dir1");
    create_test_directory("tests/dir2");
    create_test_file("tests/dir1/file1.txt", "Content A");
    create_test_file("tests/dir2/file1.txt", "Content A");
    create_test_file("tests/dir2/file2.txt", "Additional content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");

    EXPECT_NE(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

