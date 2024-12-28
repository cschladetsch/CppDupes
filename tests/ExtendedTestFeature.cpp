#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include "../include/FileHashMapper.hpp"
#include "../include/DirectoryComparer.hpp"

namespace fs = std::filesystem;

class ExtendedFileTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        cleanupDirectories();
        setupDirectories();
        total_files = 0;
        total_bytes = 0;
    }

    void TearDown() override {
        cleanupDirectories();
    }

    void cleanupDirectories() {
        const std::vector<std::string> dirs = {"test_dir1", "test_dir2", "test_dir3", "test_dir4"};
        for (const auto& dir : dirs) {
            if (fs::exists(dir)) {
                fs::remove_all(dir);
            }
        }
    }

    void setupDirectories() {
        const std::vector<std::string> dirs = {"test_dir1", "test_dir2", "test_dir3", "test_dir4"};
        for (const auto& dir : dirs) {
            fs::create_directory(dir);
        }
    }

    void writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }

    void writeBinaryFile(const std::string& path, const std::vector<uint8_t>& content) {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(content.data()), content.size());
        file.close();
    }

    std::string readFile(const std::string& path) {
        std::ifstream file(path);
        return std::string(std::istreambuf_iterator<char>(file),
                          std::istreambuf_iterator<char>());
    }
};

// Test 1: Handles extremely large number of small files
TEST_F(ExtendedFileTestFixture, HandlesLargeNumberOfFiles) {
    const int NUM_FILES = 1000;
    for (int i = 0; i < NUM_FILES; ++i) {
        writeFile("test_dir1/file" + std::to_string(i) + ".txt", "small content");
    }

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), NUM_FILES);
}

// Test 2: Handles files with only whitespace differences
TEST_F(ExtendedFileTestFixture, HandlesWhitespaceDifferences) {
    writeFile("test_dir1/file1.txt", "content  with  spaces");
    writeFile("test_dir2/file1.txt", "content with spaces");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlyDifferent, {}));
}

// Test 3: Tests directory with mixed case filenames
TEST_F(ExtendedFileTestFixture, HandlesMixedCaseFilenames) {
    writeFile("test_dir1/File.txt", "content");
    writeFile("test_dir1/file.txt", "content");
    writeFile("test_dir1/FILE.txt", "content");

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), 3);
}

// Test 4: Tests handling of files with special characters
TEST_F(ExtendedFileTestFixture, HandlesSpecialCharacterFiles) {
    writeFile("test_dir1/file~!@#$%^&()_+.txt", "content");
    writeFile("test_dir1/file[]{};',.txt", "content");

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), 2);
}

// Test 5: Tests processing of zero-byte files in multiple directories
TEST_F(ExtendedFileTestFixture, HandlesMultipleZeroByteFiles) {
    writeFile("test_dir1/empty1.txt", "");
    writeFile("test_dir2/empty2.txt", "");
    writeFile("test_dir3/empty3.txt", "");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2", "test_dir3"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, {}));
}

// Test 6: Tests directory with maximum path length
TEST_F(ExtendedFileTestFixture, HandlesMaxPathLength) {
    std::string long_dir = "test_dir1";
    std::string filename(100, 'a');  // Long filename
    
    for (int i = 0; i < 10; ++i) {
        long_dir += "/" + std::string(20, static_cast<char>('a' + i));
        fs::create_directories(long_dir);
    }
    
    writeFile(long_dir + "/" + filename + ".txt", "content");
    
    FileHashMapper mapper;
    EXPECT_NO_THROW(mapper.process_directory("test_dir1"));
}

// Test 7: Tests handling of hidden files
TEST_F(ExtendedFileTestFixture, HandlesHiddenFiles) {
    writeFile("test_dir1/.hidden1", "content1");
    writeFile("test_dir1/.hidden2", "content2");

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), 2);
}

// Test 8: Tests identical files with different timestamps
TEST_F(ExtendedFileTestFixture, HandlesIdenticalFilesWithDifferentTimestamps) {
    writeFile("test_dir1/file1.txt", "content");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    writeFile("test_dir2/file1.txt", "content");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlySame, {}));
}

// Test 9: Tests handling of files with null bytes
TEST_F(ExtendedFileTestFixture, HandlesFilesWithNullBytes) {
    std::vector<uint8_t> content = {'H', 'e', 'l', 'l', 'o', 0, 'W', 'o', 'r', 'l', 'd'};
    writeBinaryFile("test_dir1/binary1.bin", content);
    writeBinaryFile("test_dir2/binary2.bin", content);

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, {}));
}

// Test 10: Tests partial file content matches
TEST_F(ExtendedFileTestFixture, HandlesPartialContentMatches) {
    writeFile("test_dir1/file1.txt", "common content plus extra");
    writeFile("test_dir2/file1.txt", "common content");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlyDifferent, {}));
}

// Test 11: Tests nested directory exclusion
TEST_F(ExtendedFileTestFixture, HandlesNestedExclusions) {
    fs::create_directories("test_dir1/exclude/nested");
    fs::create_directories("test_dir2/exclude/nested");
    
    writeFile("test_dir1/exclude/nested/file.txt", "content");
    writeFile("test_dir2/exclude/nested/file.txt", "different");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    std::vector<std::string> exclude = {"exclude"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, exclude));
}

// Test 12: Tests handling of filesystem loops
TEST_F(ExtendedFileTestFixture, HandlesFilesystemLoops) {
    fs::create_directories("test_dir1/loop");
    fs::create_symlink("../loop", "test_dir1/loop/loop");

    FileHashMapper mapper;
    EXPECT_NO_THROW(mapper.process_directory("test_dir1"));
}

// Test 13: Tests very large file comparison
TEST_F(ExtendedFileTestFixture, HandlesVeryLargeFileComparison) {
    const size_t size = 100 * 1024 * 1024; // 100MB
    std::string large_content(size, 'X');
    
    writeFile("test_dir1/large.txt", large_content);
    writeFile("test_dir2/large.txt", large_content);

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, {}));
}

// Test 14: Tests handling of multiple directory levels with same names
TEST_F(ExtendedFileTestFixture, HandlesRecursiveSameNames) {
    fs::create_directories("test_dir1/dir/dir/dir");
    fs::create_directories("test_dir2/dir/dir/dir");
    
    writeFile("test_dir1/dir/dir/dir/file.txt", "content");
    writeFile("test_dir2/dir/dir/dir/file.txt", "content");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, {}));
}

// Test 15: Tests handling of read-only files
TEST_F(ExtendedFileTestFixture, HandlesReadOnlyFiles) {
    writeFile("test_dir1/readonly.txt", "content");
    fs::permissions("test_dir1/readonly.txt", 
        fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read,
        fs::perm_options::replace);

    FileHashMapper mapper;
    EXPECT_NO_THROW(mapper.process_directory("test_dir1"));
}

// Test 16: Tests mixed line endings
TEST_F(ExtendedFileTestFixture, HandlesMixedLineEndings) {
    writeFile("test_dir1/unix.txt", "line1\nline2\nline3");
    writeFile("test_dir2/windows.txt", "line1\r\nline2\r\nline3");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlyDifferent, {}));
}

// Test 17: Tests handling of file type changes
TEST_F(ExtendedFileTestFixture, HandlesFileTypeChanges) {
    // Create a regular file in dir1 and a directory with the same name in dir2
    writeFile("test_dir1/name", "content");
    fs::create_directory("test_dir2/name");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlyDifferent, {}));
}

// Test 18: Tests directory comparison with non-recursive mode
TEST_F(ExtendedFileTestFixture, HandlesNonRecursiveComparison) {
    fs::create_directories("test_dir1/subdir");
    fs::create_directories("test_dir2/subdir");
    
    writeFile("test_dir1/file.txt", "content");
    writeFile("test_dir2/file.txt", "content");
    writeFile("test_dir1/subdir/nested.txt", "different");
    writeFile("test_dir2/subdir/nested.txt", "content");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, {"subdir"}));
}

// Test 19: Tests handling of multiple duplicate sets
TEST_F(ExtendedFileTestFixture, HandlesMultipleDuplicateSets) {
    // Create first set of duplicates
    writeFile("test_dir1/set1_a.txt", "content1");
    writeFile("test_dir2/set1_b.txt", "content1");
    writeFile("test_dir3/set1_c.txt", "content1");
    
    // Create second set of duplicates
    writeFile("test_dir1/set2_a.txt", "content2");
    writeFile("test_dir2/set2_b.txt", "content2");
    
    // Create unique files
    writeFile("test_dir1/unique.txt", "unique1");
    writeFile("test_dir2/unique.txt", "unique2");

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2", "test_dir3"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::OnlySame, {}));
}

// Test 20: Tests atomic counters in heavy load
TEST_F(ExtendedFileTestFixture, HandlesAtomicCountersInHeavyLoad) {
    const int NUM_FILES = 1000;
    const int NUM_DIRS = 4;
    
    // Create files in multiple directories simultaneously
    std::vector<std::thread> threads;
    for (int dir = 1; dir <= NUM_DIRS; ++dir) {
        threads.emplace_back([this, dir, NUM_FILES]() {
            for (int i = 0; i < NUM_FILES; ++i) {
                writeFile("test_dir" + std::to_string(dir) + "/file" + 
                         std::to_string(i) + ".txt", "content");
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    std::vector<fs::path> dirs = {"test_dir1", "test_dir2", "test_dir3", "test_dir4"};
    EXPECT_NO_THROW(DirectoryComparer::compare_directories(dirs, ComparisonMode::All, {}));
}
