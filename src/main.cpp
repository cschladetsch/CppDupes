#include "DirectoryComparer.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <fstream>
#include <ctime>

// Performance measurement structure
struct PerformanceResult {
    double total_time_ms = 0.0;
    std::vector<double> individual_times;
};

// Run comparison and measure performance
PerformanceResult run_comparison_with_timing(
    const std::vector<std::filesystem::path>& directories, 
    ComparisonMode mode, 
    const std::vector<std::string>& exclude_folders
) {
    PerformanceResult results;
    
    // Run the comparison and measure time
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        DirectoryComparer::compare_directories(directories, mode, exclude_folders);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        throw;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    // Calculate duration
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    results.total_time_ms = duration.count() / 1000.0;
    results.individual_times.push_back(results.total_time_ms);
    
    return results;
}

// Function to log times to file
void log_times_to_file(
    int repetitions, 
    double mean_time, 
    double stdev, 
    const std::vector<std::filesystem::path>& directories,
    ComparisonMode mode,
    const std::vector<double>& individual_times
) {
    // Open file in append mode
    std::ofstream log_file("time-times.txt", std::ios_base::app);
    
    if (!log_file.is_open()) {
        std::cerr << "Warning: Could not open time-times.txt for writing\n";
        return;
    }

    // Get current timestamp
    std::time_t now = std::time(nullptr);
    char timestamp[26];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // Convert mode to string
    std::string mode_str;
    switch (mode) {
        case ComparisonMode::All: mode_str = "all"; break;
        case ComparisonMode::OnlyDifferent: mode_str = "different"; break;
        case ComparisonMode::OnlySame: mode_str = "same"; break;
        case ComparisonMode::OnlyUnique: mode_str = "unique"; break;
    }

    // Construct directory list string
    std::string dir_list;
    for (const auto& dir : directories) {
        dir_list += dir.string() + " ";
    }

    // Log entry format:
    // [Timestamp] Repetitions: X, Mean: Y ms, StdDev: Z ms, Mode: M, Directories: D
    log_file << "[" << timestamp << "] "
             << "Repetitions: " << repetitions << ", "
             << "Mean: " << std::fixed << std::setprecision(3) << mean_time << " ms, "
             << "StdDev: " << std::fixed << std::setprecision(3) << stdev << " ms, "
             << "Mode: " << mode_str << ", "
             << "Directories: " << dir_list << "\n";

    // Optionally log individual run times
    if (repetitions > 1) {
        log_file << "Individual Times:";
        for (double time : individual_times) {
            log_file << " " << std::fixed << std::setprecision(3) << time;
        }
        log_file << "\n";
    }
}

int main(int argc, char* argv[]) {
    // Default values
    int repetitions = 1;
    
    // Validate command-line arguments
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] 
                  << " <mode> [-r <repetitions>] <directory1> [<directory2> ...]\n";
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
    int dir_start_index = 2;

    // Check for repetition flag
    if (argc > 2 && std::string(argv[2]) == "-r") {
        if (argc < 5) {
            std::cerr << "Error: -r requires a number of repetitions\n";
            return 1;
        }
        try {
            repetitions = std::stoi(argv[3]);
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid number of repetitions\n";
            return 1;
        }
        dir_start_index = 4;
    }

    // Parse comparison mode
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
    for (int i = dir_start_index; i < argc; ++i) {
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
        // Performance tracking
        PerformanceResult overall_results;
        
        // Run comparisons
        for (int i = 0; i < repetitions; ++i) {
            auto result = run_comparison_with_timing(directories, mode, exclude_folders);
            
            overall_results.total_time_ms += result.total_time_ms;
            overall_results.individual_times.push_back(result.total_time_ms);
        }

        // Report performance
        if (repetitions > 1) {
            // Calculate statistics
            double mean = overall_results.total_time_ms / repetitions;
            
            // Calculate standard deviation
            double sq_sum = std::accumulate(
                overall_results.individual_times.begin(), 
                overall_results.individual_times.end(), 
                0.0, 
                [mean](double acc, double value) {
                    return acc + std::pow(value - mean, 2);
                }
            );
            double stdev = std::sqrt(sq_sum / repetitions);

            // Print performance report
            std::cout << "\nPerformance Report:\n";
            std::cout << "  Repetitions: " << repetitions << "\n";
            std::cout << "  Mean Time:   " << std::fixed << std::setprecision(3) 
                      << mean << " ms\n";
            std::cout << "  Std Dev:     " << std::fixed << std::setprecision(3) 
                      << stdev << " ms\n";

            // Print individual times
            std::cout << "  Individual Times: ";
            for (double time : overall_results.individual_times) {
                std::cout << std::fixed << std::setprecision(3) << time << " ";
            }
            std::cout << " ms\n";

            // Log times to file
            log_times_to_file(repetitions, mean, stdev, directories, mode, overall_results.individual_times);
        }
        else {
            // If only one run, just print that time
            std::cout << "Execution Time: " 
                      << std::fixed << std::setprecision(3) 
                      << overall_results.total_time_ms << " ms\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
