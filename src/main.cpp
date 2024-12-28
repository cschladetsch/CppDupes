#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "FileHashMapper.hpp"

namespace fs = std::filesystem;
namespace po = boost::program_options;

enum class ComparisonMode {
    All,
    OnlyDifferent,
    OnlySame,
    OnlyUnique
};

void compare_directories(const std::vector<fs::path>& directories, ComparisonMode mode) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> all_hashes;

    // Process each directory
    for (const auto& dir : directories) {
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            std::cerr << "Error: Invalid directory path: " << dir << std::endl;
            continue;
        }

        FileHashMapper mapper;
        mapper.process_directory(dir);
        all_hashes[dir.string()] = mapper.get_file_hashes();
    }

    // Compare hashes across directories
    std::cout << "Comparing file hashes across directories:\n";
    for (const auto& [dir1, hashes1] : all_hashes) {
        for (const auto& [dir2, hashes2] : all_hashes) {
            if (dir1 >= dir2) continue; // Avoid duplicate comparisons

            std::cout << "\nComparing " << dir1 << " and " << dir2 << ":\n";
            for (const auto& [file1, hash1] : hashes1) {
                auto it = hashes2.find(file1);
                if (it != hashes2.end()) {
                    if (hash1 == it->second) {
                        if (mode == ComparisonMode::All || mode == ComparisonMode::OnlySame) {
                            std::cout << "Match: " << file1 << " [Hash: " << hash1 << "]\n";
                        }
                    } else {
                        if (mode == ComparisonMode::All || mode == ComparisonMode::OnlyDifferent) {
                            std::cout << "Mismatch: " << file1 << "\n  Hash1: " << hash1 << "\n  Hash2: " << it->second << "\n";
                        }
                    }
                } else {
                    if (mode == ComparisonMode::All || mode == ComparisonMode::OnlyUnique) {
                        std::cout << "File only in " << dir1 << ": " << file1 << "\n";
                    }
                }
            }

            for (const auto& [file2, hash2] : hashes2) {
                if (hashes1.find(file2) == hashes1.end()) {
                    if (mode == ComparisonMode::All || mode == ComparisonMode::OnlyUnique) {
                        std::cout << "File only in " << dir2 << ": " << file2 << "\n";
                    }
                }
            }
        }
    }
}

ComparisonMode parse_mode(const std::string& mode_str) {
    if (mode_str == "different") return ComparisonMode::OnlyDifferent;
    if (mode_str == "same") return ComparisonMode::OnlySame;
    if (mode_str == "unique") return ComparisonMode::OnlyUnique;
    return ComparisonMode::All;
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("mode,m", po::value<std::string>()->default_value("all"), "Comparison mode: all, different, same, unique")
        ("directories", po::value<std::vector<std::string>>()->multitoken(), "Directories to compare");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error& ex) {
        std::cerr << "Error parsing command line: " << ex.what() << "\n";
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    std::string mode_str = vm["mode"].as<std::string>();
    ComparisonMode mode = parse_mode(mode_str);

    if (!vm.count("directories")) {
        std::cerr << "Error: At least two directories must be provided for comparison." << std::endl;
        return 1;
    }

    std::vector<std::string> directory_strings = vm["directories"].as<std::vector<std::string>>();
    if (directory_strings.size() < 2) {
        std::cerr << "Error: At least two directories must be provided for comparison." << std::endl;
        return 1;
    }

    std::vector<fs::path> directories;
    for (const auto& dir : directory_strings) {
        directories.emplace_back(dir);
    }

    compare_directories(directories, mode);

    return 0;
}
