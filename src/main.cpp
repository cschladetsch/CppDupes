#include "DirectoryComparer.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    // Validate command-line arguments
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <mode> <directory1> [<directory2> ...]\n";
        std::cerr << "Modes:\n";
        std::cerr << "  all\n";
        std::cerr << "  different\n";
        std::cerr << "  same\n";
        std::cerr << "  unique\n";
        return 1;
    }

    // Parse mode
    ComparisonMode mode;
    std::string mode_arg = argv[1];
    if (mode_arg == "all") {
        mode = ComparisonMode::All;
    } else if (mode_arg == "different") {
        mode = ComparisonMode::OnlyDifferent;
    } else if (mode_arg == "same") {
        mode = ComparisonMode::OnlySame;
    } else if (mode_arg == "unique") {
        mode = ComparisonMode::OnlyUnique;
    } else {
        std::cerr << "Invalid mode. Choose: all, different, same, or unique.\n";
        return 1;
    }

    // Collect directories
    std::vector<std::filesystem::path> directories;
    
    // Collect directory paths from command line
    for (int i = 2; i < argc; ++i) {
        std::filesystem::path dir(argv[i]);
        
        // Validate directory
        if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
            std::cerr << "Error: " << dir << " is not a valid directory.\n";
            return 1;
        }
        
        directories.push_back(dir);
    }

    // Exclude folders (optional)
    std::vector<std::string> exclude_folders = {".git"};

    try {
        // Perform directory comparison
        DirectoryComparer::compare_directories(directories, mode, exclude_folders);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
