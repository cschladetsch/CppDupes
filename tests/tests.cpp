#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "FileHashMapper.hpp"

namespace fs = std::filesystem;

class FileHashMapperTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        if (fs::exists("test_data")) {
            fs::remove_all("test_data");
        }
        fs::create_directory("test_data");
        fs::create_directory("test_data/dir1");
        fs::create_directory("test_data/dir2");
    }

    void TearDown() override {
        if (fs::exists("test_data")) {
            fs::remove_all("test_data");
        }
    }

    void writeFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        file << content;
        file.close();
    }
};

TEST_F(FileHashMapperTestFixture, ExactDuplicatesInDifferentDirectories) {
    writeFile("test_data/dir1/file1.txt", "test content");
    writeFile("test_data/dir2/file2.txt", "test content");

    FileHashMapper mapper;
    mapper.process_directory("test_data");
    
    auto hashes = mapper.get_file_hashes();
    
    // Group files by hash to find duplicates
    std::unordered_map<std::string, std::vector<std::string>> hashGroups;
    for (const auto& [path, hash] : hashes) {
        hashGroups[hash].push_back(path);
    }
    
    // Find groups with more than one file (duplicates)
    bool hasDuplicates = false;
    for (const auto& [hash, files] : hashGroups) {
        if (files.size() > 1) {
            hasDuplicates = true;
            break;
        }
    }
    
    EXPECT_TRUE(hasDuplicates);
    EXPECT_EQ(mapper.get_file_count(), 2);
}

TEST_F(FileHashMapperTestFixture, NoDuplicatesWithDifferentContent) {
    writeFile("test_data/dir1/file1.txt", "content1");
    writeFile("test_data/dir2/file2.txt", "content2");

    FileHashMapper mapper;
    mapper.process_directory("test_data");
    
    auto hashes = mapper.get_file_hashes();
    
    // Group files by hash
    std::unordered_map<std::string, std::vector<std::string>> hashGroups;
    for (const auto& [path, hash] : hashes) {
        hashGroups[hash].push_back(path);
    }
    
    // Check that no hash has more than one file
    bool hasDuplicates = false;
    for (const auto& [hash, files] : hashGroups) {
        if (files.size() > 1) {
            hasDuplicates = true;
            break;
        }
    }
    
    EXPECT_FALSE(hasDuplicates);
    EXPECT_EQ(mapper.get_file_count(), 2);
}

TEST_F(FileHashMapperTestFixture, HandlesEmptyDirectory) {
    FileHashMapper mapper;
    mapper.process_directory("test_data");
    
    EXPECT_EQ(mapper.get_file_count(), 0);
    EXPECT_EQ(mapper.get_total_size(), 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
