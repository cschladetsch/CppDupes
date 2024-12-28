#include "DirectoryComparer.hpp"
#include <iostream>
#include <boost/program_options.hpp>
#include <ctime>

namespace po = boost::program_options;

const std::string VERSION = "1.0.0";

int main(int argc, char* argv[]) {
    // Get the current time
    std::time_t now = std::time(nullptr);
    char time_str[100];
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
	std::cout << "Built on: " << time_str << std::endl;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Show help message")
        ("version,v", "Show version information")
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

    if (vm.count("version")) {
        std::cout << "FSF Application Version: " << VERSION << std::endl;
        std::cout << "Built on: " << time_str << std::endl;
        return 0;
    }

    std::string mode_str = vm["mode"].as<std::string>();
    ComparisonMode mode = ComparisonMode::All;
    if (mode_str == "different") mode = ComparisonMode::OnlyDifferent;
    else if (mode_str == "same") mode = ComparisonMode::OnlySame;
    else if (mode_str == "unique") mode = ComparisonMode::OnlyUnique;

    if (!vm.count("directories")) {
        std::cerr << "Error: At least two directories must be provided for comparison." << std::endl;
        return 1;
    }

    std::vector<std::string> dir_strings = vm["directories"].as<std::vector<std::string>>();
    if (dir_strings.size() < 2) {
        std::cerr << "Error: At least two directories must be provided for comparison." << std::endl;
        return 1;
    }

    std::vector<fs::path> directories;
    for (const auto& dir : dir_strings) {
        directories.emplace_back(dir);
    }

    DirectoryComparer::compare_directories(directories, mode);

    return 0;
}

