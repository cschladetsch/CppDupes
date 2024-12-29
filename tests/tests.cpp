#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <random>
#include <chrono>
#include <algorithm>
#include "FileHashMapper.hpp"

#include "rang.hpp"
#include "DirectoryComparer.hpp"
#include "CustomTestListener.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    // Remove default listener
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    auto default_listener = listeners.Release(listeners.default_result_printer());

    // Add custom verbose listener with color output
    listeners.Append(new CustomTestListener(default_listener));

    return RUN_ALL_TESTS();
}

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

// Test case: Verify file creation and content
TEST_F(ExtendedFileTests, FileCreationAndContent) {
    std::string test_path = "test_dir1/test_file.txt";
    std::string test_content = "This is a test file.";
    writeFile(test_path, test_content);

    std::ifstream file(test_path);
    ASSERT_TRUE(file.is_open());
    std::string content;
    std::getline(file, content);
    file.close();

    EXPECT_EQ(content, test_content);
}

// Test case: Verify binary file creation
TEST_F(ExtendedFileTests, BinaryFileCreation) {
    std::string test_path = "test_dir1/test_binary.bin";
    std::vector<uint8_t> test_content = {0xDE, 0xAD, 0xBE, 0xEF};
    writeBinaryFile(test_path, test_content);

    std::ifstream file(test_path, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    std::vector<uint8_t> read_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    EXPECT_EQ(read_content, test_content);
}

// Test case: Verify directory creation
TEST_F(ExtendedFileTests, DirectoryCreation) {
    std::string test_dir = "test_dir1/subdir";
    fs::create_directory(test_dir);
    EXPECT_TRUE(fs::exists(test_dir));
}

// Test case: Verify cleanup of directories
TEST_F(ExtendedFileTests, CleanupDirectories) {
    std::string test_dir = "test_dir1/cleanup_test";
    fs::create_directory(test_dir);
    EXPECT_TRUE(fs::exists(test_dir));

    cleanupDirectories();
    EXPECT_FALSE(fs::exists(test_dir));
}

// Test case: Verify random content generation
TEST_F(ExtendedFileTests, RandomContentGeneration) {
    size_t content_size = 16;
    std::string content = generateRandomContent(content_size);
    EXPECT_EQ(content.size(), content_size);
}

// Test case: Verify binary content generation
TEST_F(ExtendedFileTests, BinaryContentGeneration) {
    size_t content_size = 16;
    std::vector<uint8_t> content = generateRandomBinaryContent(content_size);
    EXPECT_EQ(content.size(), content_size);
}

