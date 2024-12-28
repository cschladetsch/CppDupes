# FSF Project

## Overview
FSF is a project designed to handle file comparisons using MD5 hashes, while providing modular and testable architecture. The project is written in modern C++20, leveraging OpenSSL for cryptographic operations, Boost for command-line argument parsing, and Google Test for unit testing.

## Features
- Compute and compare MD5 hashes for files in directories.
- Supports comparing any number of directories specified on the command line.
- Options to display only different files, same files, or files unique to one directory.
- Modular architecture with `src` and `tests` directories for clear separation of concerns.
- Boost program-options for arguments.
- Google Test integration for robust testing.
- C++20 support for modern features and better performance.

## Directory Structure
```
fsf/
├── CMakeLists.txt          # Root CMake configuration
├── src/                   # Source files for the main application
│   ├── CMakeLists.txt      # CMake configuration for src
│   ├── main.cpp            # Entry point for the application
│   └── FileHashMapper.cpp  # Core logic for hashing and file processing
├── tests/                 # Unit tests for the application
│   ├── CMakeLists.txt      # CMake configuration for tests
│   └── tests.cpp           # Test cases using Google Test
└── include/               # Shared headers
    └── FileHashMapper.hpp  # Declaration of FileHashMapper
```

## Requirements
- **CMake**: Version 3.10 or higher
- **C++ Compiler**: GCC, Clang, or MSVC supporting C++20
- **Dependencies**:
  - Google Test (GTest)
  - OpenSSL
  - Boost Program Options

## Setup and Build
### Cloning the Repository
```bash
git clone <repository-url>
cd fsf
```

### Building and Running the Project
Use the provided `b` script to manage builds. The following options are available and can be combined:

- `-c`: Clean the build directory.
- `-b`: Build the project.
- `-t`: Run tests without rebuilding.

#### Examples
- **Clean and build**:
  ```bash
  ./b -cb
  ```
- **Clean, build, and run tests**:
  ```bash
  ./b -cbt
  ```
- **Build and run tests**:
  ```bash
  ./b -bt
  ```

The compiled binary (`fsf`) will be located in `~/bin`. Esnure this is added to your PATH.

### Main Application Help Output
Run the application with `--help` to see available options:
```bash
~/bin/fsf --help
```

#### Example Output
```
Allowed options:
  -h [ --help ]          Show help message
  -m [ --mode ] arg      Comparison mode: all, different, same, unique
  --directories arg      Directories to compare
```

#### Usage Examples:
- Compare all files in two directories:
  ```bash
  ~/bin/fsf --directories /path/to/dir1 /path/to/dir2
  ```
- Show only files that differ:
  ```bash
  ~/bin/fsf --mode different --directories /path/to/dir1 /path/to/dir2
  ```
- Show unique files across three directories:
  ```bash
  ~/bin/fsf --mode unique --directories /path/to/dir1 /path/to/dir2 /path/to/dir3
  ```

### Boost Dependency
Ensure Boost libraries are installed:
```bash
sudo apt-get install libboost-program-options-dev
```

### Running Tests
To build and run tests using the `b` script:
```bash
./b -cbt
```
This cleans, builds, and runs the test suite, providing detailed output in case of failures.

To run tests without rebuilding:
```bash
./b -t
```
This runs the test suite only.

## Contributing
1. Fork the repository and create a new branch for your feature or bugfix.
2. Submit a pull request with a detailed explanation of the changes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

## Acknowledgments
- **Google Test**: For providing the testing framework.
- **OpenSSL**: For cryptographic operations.
- **Boost**: For program options parsing.
- **CMake**: For build system management.

