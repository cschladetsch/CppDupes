#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <random>
#include <chrono>
#include <algorithm>
#include "../include/FileHashMapper.hpp"
#include "../include/DirectoryComparer.hpp"

namespace fs = std::filesystem;

class ExtendedFileTests : public ::testing::Test {
protected:
    void SetUp() override {
        cleanupDirectories();
        setupDirectories();
    }

    void TearDown() override {
        cleanupDirectories();
    }

    void cleanupDirectories() {
        std::vector<std::string> dirs = {
            "test_dir1", "test_dir2", "test_dir3", "test_dir4",
            "temp_dir", "special_dir", "unicode_dir"
        };
        for (const auto& dir : dirs) {
            if (fs::exists(dir)) {
                fs::remove_all(dir);
            }
        }
    }

    void setupDirectories() {
        std::vector<std::string> dirs = {
            "test_dir1", "test_dir2", "test_dir3", "test_dir4",
            "temp_dir", "special_dir", "unicode_dir"
        };
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

    std::string generateRandomContent(size_t size) {
        static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
        std::string result;
        result.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            result += charset[dis(gen)];
        }
        return result;
    }

    std::vector<uint8_t> generateRandomBinaryContent(size_t size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        std::vector<uint8_t> result(size);
        for (size_t i = 0; i < size; ++i) {
            result[i] = static_cast<uint8_t>(dis(gen));
        }
        return result;
    }
};

// Test handling of huge sparse files
TEST_F(ExtendedFileTests, HandlesSparseFiles) {
    const size_t file_size = 1024 * 1024 * 10; // 10MB
    std::string sparse_path = "test_dir1/sparse.dat";
    
    std::ofstream file(sparse_path, std::ios::binary);
    file.seekp(file_size - 1);
    file.put('\0');
    file.close();

    FileHashMapper mapper;
    EXPECT_NO_THROW(mapper.compute_md5(sparse_path));
}

// Test handling of files with special Unicode names
TEST_F(ExtendedFileTests, HandlesComplexUnicodeFilenames) {
    std::vector<std::string> unicode_names = {
        "unicode_dir/测试文件.txt",
        "unicode_dir/테스트파일.txt",
        "unicode_dir/тестовый файл.txt",
        "unicode_dir/file.txt"
    };

    for (const auto& name : unicode_names) {
        writeFile(name, "unicode content");
    }

    FileHashMapper mapper;
    mapper.process_directory("unicode_dir");
    EXPECT_EQ(mapper.get_file_count(), unicode_names.size());
}

// Test concurrent directory access
TEST_F(ExtendedFileTests, HandlesConcurrentAccess) {
    const int num_threads = 4;
    const int files_per_thread = 100;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, t, files_per_thread]() {
            for (int i = 0; i < files_per_thread; ++i) {
                std::string filename = "test_dir1/thread" + std::to_string(t) + 
                                     "_file" + std::to_string(i) + ".txt";
                writeFile(filename, generateRandomContent(100));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), num_threads * files_per_thread);
}

// Test handling of multiple file sizes
TEST_F(ExtendedFileTests, HandlesMultipleFileSizes) {
    std::vector<size_t> sizes = {0, 1, 100, 1024, 1024*1024};
    int file_count = 0;
    
    for (const auto& size : sizes) {
        std::string filename = "test_dir1/file_" + std::to_string(size) + ".txt";
        writeFile(filename, generateRandomContent(size));
        file_count++;
    }

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), file_count);
}

// Test special directory names
TEST_F(ExtendedFileTests, HandlesSpecialDirectoryNames) {
    std::vector<std::string> special_dirs = {
        "test_dir1/space dir",
        "test_dir1/tab\tdir",
        "test_dir1/multi\nline",
        "test_dir1/special#@!dir"
    };

    for (const auto& dir : special_dirs) {
        fs::create_directories(dir);
        writeFile(dir + "/file.txt", "content");
    }

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), special_dirs.size());
}

// Test file corruption handling
TEST_F(ExtendedFileTests, HandlesFileCorruption) {
    std::string filename = "test_dir1/corrupt.txt";
    writeFile(filename, "initial content");
    
    std::string hash1 = FileHashMapper::compute_md5(filename);
    
    // Simulate corruption
    writeFile(filename, "corrupted content");
    
    std::string hash2 = FileHashMapper::compute_md5(filename);
    EXPECT_NE(hash1, hash2);
}

// Test directory depth limits
TEST_F(ExtendedFileTests, HandlesDirectoryDepth) {
    std::string path = "test_dir1";
    const int max_depth = 20;
    
    for (int i = 0; i < max_depth; ++i) {
        path += "/level" + std::to_string(i);
        fs::create_directories(path);
        writeFile(path + "/file.txt", "content");
    }

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), max_depth);
}

// Test handling of symbolic link cycles
TEST_F(ExtendedFileTests, HandlesSymlinkCycles) {
    fs::create_directories("test_dir1/a/b/c");
    writeFile("test_dir1/a/b/c/file.txt", "content");
    
    // Create a cycle
    fs::create_symlink("../../a", "test_dir1/a/b/link");

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), 1);  // Only count the real file
}

// Test with maximum allowed filenames
TEST_F(ExtendedFileTests, HandlesMaxFilenames) {
    std::string max_filename(255, 'a');  // Max filename length in most filesystems
    std::string filepath = "test_dir1/" + max_filename;
    
    writeFile(filepath, "content");

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_EQ(mapper.get_file_count(), 1);
}

// Test with mixed file types
TEST_F(ExtendedFileTests, HandlesMixedFileTypes) {
    // Regular file
    writeFile("test_dir1/regular.txt", "content");
    
    // Binary file
    writeBinaryFile("test_dir1/binary.bin", {0x00, 0xFF, 0x42});
    
    // Empty file
    writeFile("test_dir1/empty.txt", "");
    
    #ifdef __linux__
    // Named pipe (FIFO)
    std::string fifo_path = "test_dir1/test.fifo";
    mkfifo(fifo_path.c_str(), 0666);
    #endif

    FileHashMapper mapper;
    mapper.process_directory("test_dir1");
    EXPECT_GE(mapper.get_file_count(), 3);  // At least regular, binary, and empty
}
