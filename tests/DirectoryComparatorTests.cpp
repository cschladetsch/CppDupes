#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "../include/DirectoryComparer.hpp"

namespace fs = std::filesystem;

class DirectoryComparerTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directories if they don't exist
        if (fs::exists("dir1")) {
            fs::remove_all("dir1");
        }
        if (fs::exists("dir2")) {
            fs::remove_all("dir2");
        }
        fs::create_directory("dir1");
        fs::create_directory("dir2");
    }

    void TearDown() override {
        // Cleanup
        if (fs::exists("dir1")) {
            fs::remove_all("dir1");
        }
        if (fs::exists("dir2")) {
            fs::remove_all("dir2");
        }
    }

    void writeTestFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }
};

TEST_F(DirectoryComparerTests, CompareEmptyDirectories) {
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
}

TEST_F(DirectoryComparerTests, CompareDirectoriesWithDifferentContent) {
    writeTestFile("dir1/test.txt", "content1");
    writeTestFile("dir2/test.txt", "different_content");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlyDifferent, exclude));
}

TEST_F(DirectoryComparerTests, CompareNestedDirectories) {
    fs::create_directories("dir1/nested");
    fs::create_directories("dir2/nested");
    
    writeTestFile("dir1/nested/test.txt", "same_content");
    writeTestFile("dir2/nested/test.txt", "same_content");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlySame, exclude));
}

TEST_F(DirectoryComparerTests, CompareWithExcludedFolders) {
    fs::create_directories("dir1/excluded");
    fs::create_directories("dir2/excluded");
    
    writeTestFile("dir1/excluded/test.txt", "content");
    writeTestFile("dir2/excluded/test.txt", "different");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude = {"excluded"};
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
}

TEST_F(DirectoryComparerTests, CompareUniqueFiles) {
    writeTestFile("dir1/unique1.txt", "content1");
    writeTestFile("dir2/unique2.txt", "content2");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlyUnique, exclude));
}

TEST_F(DirectoryComparerTests, HandlesMissingDirectory) {
    fs::remove_all("dir2");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude),
                 std::runtime_error);
}

TEST_F(DirectoryComparerTests, HandlesSpecialCharacters) {
    writeTestFile("dir1/special!@#$%^&.txt", "content");
    writeTestFile("dir2/special!@#$%^&.txt", "content");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
}

TEST_F(DirectoryComparerTests, HandlesEmptyFiles) {
    writeTestFile("dir1/empty.txt", "");
    writeTestFile("dir2/empty.txt", "");
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
}

TEST_F(DirectoryComparerTests, HandlesManyFiles) {
    for (int i = 0; i < 100; ++i) {
        writeTestFile("dir1/file" + std::to_string(i) + ".txt", "content" + std::to_string(i));
        writeTestFile("dir2/file" + std::to_string(i) + ".txt", "content" + std::to_string(i));
    }
    
    std::vector<fs::path> dirs = {"dir1", "dir2"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
}

TEST_F(DirectoryComparerTests, HandlesMultipleDirectories) {
    fs::create_directory("dir3");
    
    writeTestFile("dir1/test.txt", "content");
    writeTestFile("dir2/test.txt", "content");
    writeTestFile("dir3/test.txt", "content");
    
    std::vector<fs::path> dirs = {"dir1", "dir2", "dir3"};
    std::vector<std::string> exclude;
    
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
    
    fs::remove_all("dir3");
}
