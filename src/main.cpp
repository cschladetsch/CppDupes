#include <iostream>
#include "FileHashMapper.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <directory1> <directory2>" << std::endl;
        return 1;
    }

    FileHashMapper mapper1, mapper2;
    mapper1.process_directory(argv[1]);
    mapper2.process_directory(argv[2]);

    auto hashes1 = mapper1.get_file_hashes();
    auto hashes2 = mapper2.get_file_hashes();

    std::cout << "Comparing file hashes:\n";
    for (const auto& [file1, hash1] : hashes1) {
        auto it = hashes2.find(file1);
        if (it != hashes2.end()) {
            if (hash1 == it->second) {
                std::cout << "Match: " << file1 << " [Hash: " << hash1 << "]\n";
            } else {
                std::cout << "Mismatch: " << file1 << "\n";
                std::cout << "  Hash1: " << hash1 << "\n";
                std::cout << "  Hash2: " << it->second << "\n";
            }
        } else {
            std::cout << "File only in directory 1: " << file1 << "\n";
        }
    }

    for (const auto& [file2, hash2] : hashes2) {
        if (hashes1.find(file2) == hashes1.end()) {
            std::cout << "File only in directory 2: " << file2 << "\n";
        }
    }

    return 0;
}

