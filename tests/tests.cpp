#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>

namespace fs = std::filesystem;

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


// Utility function to create test files
std::pair<std::string, std::string> create_test_files(const std::string& content1, const std::string& content2) {
    std::string file1 = "test_file1.txt";
    std::string file2 = "test_file2.txt";
    std::ofstream(file1) << content1;
    std::ofstream(file2) << content2;
    return {file1, file2};
}

// Test case 1: Check if files exist
TEST(FileTests, FilesExist) {
    auto [file1, file2] = create_test_files("Hello, World!", "Hello, World!");
    EXPECT_TRUE(fs::exists(file1));
    EXPECT_TRUE(fs::exists(file2));
    fs::remove(file1);
    fs::remove(file2);
}

// Test case 2: Compare file contents (identical)
TEST(FileTests, CompareIdenticalFiles) {
    auto [file1, file2] = create_test_files("Hello, World!", "Hello, World!");
    EXPECT_EQ(fs::file_size(file1), fs::file_size(file2));
    fs::remove(file1);
    fs::remove(file2);
}

// Test case 3: Compare file contents (different)
TEST(FileTests, CompareDifferentFiles) {
    auto [file1, file2] = create_test_files("Hello, World!", "Different Content");
    EXPECT_NE(fs::file_size(file1), fs::file_size(file2));
    fs::remove(file1);
    fs::remove(file2);
}

// Test case 4: Check file sizes in human-readable form
TEST(FileTests, FileSizeHumanReadable) {
    auto [file1, file2] = create_test_files("12345", "1234567890");
    EXPECT_EQ(fs::file_size(file1), 5);
    EXPECT_EQ(fs::file_size(file2), 10);
    fs::remove(file1);
    fs::remove(file2);
}

// Test case 5: Ensure file deletion
TEST(FileTests, DeleteFiles) {
    auto [file1, file2] = create_test_files("Temporary", "Temporary");
    fs::remove(file1);
    fs::remove(file2);
    EXPECT_FALSE(fs::exists(file1));
    EXPECT_FALSE(fs::exists(file2));
}


// Test case 7: Check file renaming
TEST(FileTests, RenameFile) {
    auto [file1, _] = create_test_files("Rename Me", "");
    std::string renamed_file = "renamed_file.txt";
    fs::rename(file1, renamed_file);
    EXPECT_FALSE(fs::exists(file1));
    EXPECT_TRUE(fs::exists(renamed_file));
    fs::remove(renamed_file);
}

// Test case 8: Verify file permissions
TEST(FileTests, FilePermissions) {
    auto [file1, _] = create_test_files("Permission Test", "");
    auto perms = fs::status(file1).permissions();
    EXPECT_TRUE((perms & fs::perms::owner_read) != fs::perms::none);
    fs::remove(file1);
}

// Test case 9: Create files in a new directory
TEST(FileTests, CreateInNewDirectory) {
    std::string dir = "test_dir";
    fs::create_directory(dir);
    std::string file = dir + "/file_in_dir.txt";
    std::ofstream(file) << "Content";
    EXPECT_TRUE(fs::exists(file));
    fs::remove(file);
    fs::remove(dir);
}

// Test case 10: Check file size after appending content
TEST(FileTests, AppendContent) {
    auto [file1, _] = create_test_files("Initial", "");
    std::ofstream ofs(file1, std::ios::app);
    ofs << " Added";
    ofs.close();
    EXPECT_GT(fs::file_size(file1), 7);
    fs::remove(file1);
}

// Test case 11: Verify creating nested directories
TEST(FileTests, CreateNestedDirectories) {
    std::string dir = "test_dir/nested_dir";
    fs::create_directories(dir);
    EXPECT_TRUE(fs::exists(dir));
    fs::remove_all("test_dir");
}

// Test case 12: Verify file copy
TEST(FileTests, CopyFile) {
    auto [file1, _] = create_test_files("Copy this file", "");
    std::string copied_file = "copied_file.txt";
    fs::copy(file1, copied_file);
    EXPECT_TRUE(fs::exists(copied_file));
    fs::remove(file1);
    fs::remove(copied_file);
}

// Test case 13: Verify moving a file
TEST(FileTests, MoveFile) {
    auto [file1, _] = create_test_files("Move this file", "");
    std::string moved_file = "moved_file.txt";
    fs::rename(file1, moved_file);
    EXPECT_TRUE(fs::exists(moved_file));
    EXPECT_FALSE(fs::exists(file1));
    fs::remove(moved_file);
}

// Test case 14: Verify reading file content
TEST(FileTests, ReadFileContent) {
    auto [file1, _] = create_test_files("Read this content", "");
    std::ifstream ifs(file1);
    std::string content;
    std::getline(ifs, content);
    EXPECT_EQ(content, "Read this content");
    fs::remove(file1);
}

// Test case 15: Verify clearing file content
TEST(FileTests, ClearFileContent) {
    auto [file1, _] = create_test_files("Clear this content", "");
    std::ofstream ofs(file1, std::ios::trunc);
    ofs.close();
    EXPECT_EQ(fs::file_size(file1), 0);
    fs::remove(file1);
}

// Test case 16: Verify symbolic link creation
TEST(FileTests, CreateSymbolicLink) {
    auto [file1, _] = create_test_files("Symbolic link test", "");
    std::string symlink = "symlink_file.txt";
    fs::create_symlink(file1, symlink);
    EXPECT_TRUE(fs::exists(symlink));
    fs::remove(file1);
    fs::remove(symlink);
}

// Test case 17: Verify hard link creation
TEST(FileTests, CreateHardLink) {
    auto [file1, _] = create_test_files("Hard link test", "");
    std::string hardlink = "hardlink_file.txt";
    fs::create_hard_link(file1, hardlink);
    EXPECT_TRUE(fs::exists(hardlink));
    fs::remove(file1);
    fs::remove(hardlink);
}

// Test case 18: Verify file extension
TEST(FileTests, CheckFileExtension) {
    auto [file1, _] = create_test_files("Extension test", "");
    EXPECT_EQ(fs::path(file1).extension(), ".txt");
    fs::remove(file1);
}

// Test case 19: Verify directory removal
TEST(FileTests, RemoveDirectory) {
    std::string dir = "test_dir";
    fs::create_directory(dir);
    fs::remove(dir);
    EXPECT_FALSE(fs::exists(dir));
}

// Test case 20: Verify writing binary data
TEST(FileTests, WriteBinaryData) {
    std::string file = "binary_file.bin";
    std::ofstream ofs(file, std::ios::binary);
    ofs.write("\xDE\xAD\xBE\xEF", 4);
    ofs.close();
    EXPECT_EQ(fs::file_size(file), 4);
    fs::remove(file);
}

