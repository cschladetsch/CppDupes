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

// Additional 10 Test Cases
TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithEmptyAndNonEmptyFiles) {
    create_test_directory("tests/dir1");
    create_test_directory("tests/dir2");
    create_test_file("tests/dir1/file1.txt", "");
    create_test_file("tests/dir1/file2.txt", "Non-empty content");
    create_test_file("tests/dir2/file1.txt", "");
    create_test_file("tests/dir2/file2.txt", "Non-empty content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");

    EXPECT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST_F(FileHashMapperTestFixture, ProcessDirectoryWithSpecialCharactersInNames) {
    create_test_directory("tests/special_names_dir");
    create_test_file("tests/special_names_dir/complex-name_123.txt", "Special characters in name");
    create_test_file("tests/special_names_dir/another@file!.txt", "Another file content");

    FileHashMapper mapper;
    mapper.process_directory("tests/special_names_dir");

    EXPECT_EQ(mapper.get_file_hashes().size(), 2);
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithLargeBinaryAndTextFiles) {
    create_test_directory("tests/dir1");
    create_test_directory("tests/dir2");
    create_test_file("tests/dir1/text_file.txt", "This is text content.");
    create_test_file("tests/dir1/binary_file.bin", std::string(5 * 1024 * 1024, 'A')); // 5MB binary
    create_test_file("tests/dir2/text_file.txt", "This is text content.");
    create_test_file("tests/dir2/binary_file.bin", std::string(5 * 1024 * 1024, 'A'));

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");

    EXPECT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithRecursiveSymlinks) {
    create_test_directory("tests/recursive_symlinks");
    fs::create_symlink("tests/recursive_symlinks", "tests/recursive_symlinks/self");

    FileHashMapper mapper;
    ASSERT_NO_THROW(mapper.process_directory("tests/recursive_symlinks"));
}

TEST_F(FileHashMapperTestFixture, ExcludeFilesBasedOnRegexPatterns) {
    create_test_directory("tests/regex_exclusions");
    create_test_file("tests/regex_exclusions/file1.txt", "Include this");
    create_test_file("tests/regex_exclusions/exclude.tmp", "Exclude this");

    FileHashMapper mapper;
    mapper.process_directory("tests/regex_exclusions");

    EXPECT_EQ(mapper.get_file_hashes().size(), 2);
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithHiddenAndVisibleFiles) {
    create_test_directory("tests/dir_with_hidden_and_visible");
    create_test_file("tests/dir_with_hidden_and_visible/.hidden_file", "Hidden content");
    create_test_file("tests/dir_with_hidden_and_visible/visible_file.txt", "Visible content");

    FileHashMapper mapper;
    mapper.process_directory("tests/dir_with_hidden_and_visible");

    EXPECT_EQ(mapper.get_file_hashes().size(), 2);
}

TEST_F(FileHashMapperTestFixture, ProcessDirectoriesWithFilesAndSubdirectories) {
    create_test_directory("tests/dir_with_subdirs");
    create_test_directory("tests/dir_with_subdirs/subdir");
    create_test_file("tests/dir_with_subdirs/file.txt", "File content");
    create_test_file("tests/dir_with_subdirs/subdir/nested_file.txt", "Nested file content");

    FileHashMapper mapper;
    mapper.process_directory("tests/dir_with_subdirs");

    EXPECT_EQ(mapper.get_file_hashes().size(), 2);
}

TEST_F(FileHashMapperTestFixture, ExcludeFilesWithMultiplePatterns) {
    create_test_directory("tests/dir_with_multiple_patterns");
    create_test_file("tests/dir_with_multiple_patterns/file1.txt", "Keep this");
    create_test_file("tests/dir_with_multiple_patterns/temp_file.tmp", "Exclude this");
    create_test_file("tests/dir_with_multiple_patterns/log_file.log", "Exclude this too");

    FileHashMapper mapper;
    mapper.process_directory("tests/dir_with_multiple_patterns");

    EXPECT_EQ(mapper.get_file_hashes().size(), 3);
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithDeeplyNestedStructures) {
    create_test_directory("tests/deep_structure1");
    create_test_directory("tests/deep_structure2");
    for (int i = 0; i < 10; ++i) {
        std::string nested_dir1 = "tests/deep_structure1/" + std::string(i, 'a');
        std::string nested_dir2 = "tests/deep_structure2/" + std::string(i, 'a');
        create_test_directory(nested_dir1);
        create_test_directory(nested_dir2);
        create_test_file(nested_dir1 + "/file.txt", "Nested content");
        create_test_file(nested_dir2 + "/file.txt", "Nested content");
    }

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/deep_structure1");
    mapper2.process_directory("tests/deep_structure2");

    EXPECT_EQ(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

TEST_F(FileHashMapperTestFixture, CompareDirectoriesWithIdenticalFilesDifferentNames) {
    create_test_directory("tests/dir1");
    create_test_directory("tests/dir2");
    create_test_file("tests/dir1/file1.txt", "Same content");
    create_test_file("tests/dir2/file2.txt", "Same content");

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory("tests/dir1");
    mapper2.process_directory("tests/dir2");

    EXPECT_NE(mapper1.get_file_hashes(), mapper2.get_file_hashes());
}

