#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include "FileHashMapper.hpp"

namespace fs = std::filesystem;

class FileHashMapperTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory if it doesn't exist
        if (fs::exists("test_dir")) {
            fs::remove_all("test_dir");
        }
        fs::create_directory("test_dir");
    }

    void TearDown() override {
        // Cleanup
        if (fs::exists("test_dir")) {
            fs::remove_all("test_dir");
        }
    }

    void writeTestFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }
};

TEST_F(FileHashMapperTests, EmptyFileHasConsistentHash) {
    writeTestFile("test_dir/empty.txt", "");
    
    std::string hash1 = FileHashMapper::compute_md5("test_dir/empty.txt");
    std::string hash2 = FileHashMapper::compute_md5("test_dir/empty.txt");
    
    EXPECT_EQ(hash1, hash2);
}

TEST_F(FileHashMapperTests, LargeFileHashingWorks) {
    std::string largeContent(1024 * 1024, 'A'); // 1MB file
    writeTestFile("test_dir/large.txt", largeContent);
    
    EXPECT_NO_THROW(FileHashMapper::compute_md5("test_dir/large.txt"));
}

TEST_F(FileHashMapperTests, BinaryFileHashingWorks) {
    // Create binary file
    std::ofstream file("test_dir/binary.dat", std::ios::binary);
    unsigned char binaryData[] = {0x00, 0xFF, 0x12, 0x34};
    file.write(reinterpret_cast<char*>(binaryData), sizeof(binaryData));
    file.close();
    
    EXPECT_NO_THROW(FileHashMapper::compute_md5("test_dir/binary.dat"));
}

TEST_F(FileHashMapperTests, HashChangesWithContentChange) {
    writeTestFile("test_dir/test.txt", "initial content");
    std::string hash1 = FileHashMapper::compute_md5("test_dir/test.txt");
    
    writeTestFile("test_dir/test.txt", "modified content");
    std::string hash2 = FileHashMapper::compute_md5("test_dir/test.txt");
    
    EXPECT_NE(hash1, hash2);
}

TEST_F(FileHashMapperTests, NonExistentFileThrowsException) {
    EXPECT_THROW(FileHashMapper::compute_md5("non_existent.txt"), std::runtime_error);
}

TEST_F(FileHashMapperTests, HandlesSpecialCharactersInFilename) {
    writeTestFile("test_dir/special!@#$%^&.txt", "content");
    EXPECT_NO_THROW(FileHashMapper::compute_md5("test_dir/special!@#$%^&.txt"));
}

TEST_F(FileHashMapperTests, ProcessDirectoryWorksCorrectly) {
    writeTestFile("test_dir/file1.txt", "content1");
    writeTestFile("test_dir/file2.txt", "content2");
    fs::create_directory("test_dir/subdir");
    writeTestFile("test_dir/subdir/file3.txt", "content3");
    
    FileHashMapper mapper;
    mapper.process_directory("test_dir");
    
    EXPECT_EQ(mapper.get_file_count(), 3);
    auto hashes = mapper.get_file_hashes();
    EXPECT_EQ(hashes.size(), 3);
}

TEST_F(FileHashMapperTests, HandlesZeroByteFile) {
    std::ofstream file("test_dir/zero.txt");
    file.close();
    
    EXPECT_NO_THROW(FileHashMapper::compute_md5("test_dir/zero.txt"));
}

TEST_F(FileHashMapperTests, ConsistentHashForSameContent) {
    writeTestFile("test_dir/file1.txt", "test content");
    writeTestFile("test_dir/file2.txt", "test content");
    
    EXPECT_EQ(FileHashMapper::compute_md5("test_dir/file1.txt"),
              FileHashMapper::compute_md5("test_dir/file2.txt"));
}

TEST_F(FileHashMapperTests, TotalSizeIsCorrect) {
    std::string content1(1000, 'A');
    std::string content2(2000, 'B');
    writeTestFile("test_dir/file1.txt", content1);
    writeTestFile("test_dir/file2.txt", content2);
    
    FileHashMapper mapper;
    mapper.process_directory("test_dir");
    
    EXPECT_EQ(mapper.get_total_size(), 3000);
}
