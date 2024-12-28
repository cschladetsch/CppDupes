#include <gtest/gtest.h>
#include "DirectoryComparer.hpp"
#include "FileHashMapper.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

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

// Test Cases
TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithSameContent) {
    create_test_directory("tests/dir1");
    create_test_directory("tests/dir2");
    create_test_file("tests/dir1/file.txt", "Same content");
    create_test_file("tests/dir2/file.txt", "Same content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");

    ASSERT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST_F(FileHashMapperTestFixture, CompareNestedDirectories) {
    create_test_directory("tests/nested_dir1/subdir");
    create_test_directory("tests/nested_dir2/subdir");
    create_test_file("tests/nested_dir1/subdir/file.txt", "Nested content");
    create_test_file("tests/nested_dir2/subdir/file.txt", "Nested content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/nested_dir1");
    mapper2.process_directory("tests/nested_dir2");

    ASSERT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

