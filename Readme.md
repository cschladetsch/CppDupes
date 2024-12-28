# FSF Project

## Overview
FSF is a project designed to handle file comparisons using MD5 hashes, while providing modular and testable architecture. The project is written in modern C++20, leveraging OpenSSL for cryptographic operations and Google Test for unit testing.

## Features
- Compute and compare MD5 hashes for files in directories.
- Modular architecture with `src` and `tests` directories for clear separation of concerns.
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

## Setup and Build
### Cloning the Repository
```bash
git clone <repository-url>
cd fsf
```

### Building the Project
Use the provided `b` script for managing builds. The options for `b` can be combined in any order:

- **Clean the build directory**:
  ```bash
  ./b -c
  ```
- **Build the project**:
  ```bash
  ./b -b
  ```
- **Clean and build**:
  ```bash
  ./b -cb
  ```
- **Run tests without rebuilding**:
  ```bash
  ./b -t
  ```

### Examples of Combined Options
You can combine the options of `b` in any order to perform multiple tasks in one command:

- **Clean, build, and run tests**:
  ```bash
  ./b -cbt
  ```
- **Build and run tests**:
  ```bash
  ./b -bt
  ```
- **Clean only**:
  ```bash
  ./b -c
  ```
- **Run tests only**:
  ```bash
  ./b -t
  ```

### Running the Main Application
The compiled binary for the main application (`fsf_main`) will be located in `~/bin`. To execute:
```bash
~/bin/fsf_main <directory1> <directory2>
```
This compares the files in `directory1` and `directory2` based on their MD5 hashes.

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
- **CMake**: For build system management.


