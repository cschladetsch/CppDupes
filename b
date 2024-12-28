# FSF Project

## Overview
FSF is a project designed to efficiently handle file comparisons and related functionalities. It supports robust testing using Google Test (GTest) and includes a streamlined build process with CMake.

## Features
- File comparison tools with human-readable size outputs for paired files.
- Unit tests powered by Google Test.
- Modular design for extensibility.
- Compatibility with C++20.

## Requirements
- CMake 3.10 or higher
- GCC/Clang (or equivalent compiler supporting C++20)
- Google Test library
- OpenSSL development library

## Setup and Build

### Building the Project
1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd fsf
   ```
2. Create and navigate to the build directory:
   ```bash
   mkdir build && cd build
   ```
3. Run CMake and build:
   ```bash
   cmake ..
   make
   ```

The compiled binaries will be available in the `build/bin` directory.

### Using the Build Script
The project includes a build script `b` with flexible options. You can specify options in any order and combine them as needed:
- `-c`: Clean the build directory.
- `-b`: Build the project.
- `-t`: Run tests.

Examples:
- `./b -c`: Clean only.
- `./b -b`: Build only.
- `./b -cb`: Clean and build.
- `./b -t`: Run tests.
- `./b -bct`: Build, clean, and test (order does not matter).

### Running Tests
To run the tests:
```bash
./b -t
```
Alternatively, you can execute the test binary directly:
```bash
./build/bin/fsf_tests
```

## Usage
The `fsf_main` executable provides the core functionality. Pass the required arguments to compare files or execute other tasks as needed.

## Contributing
1. Fork the repository and create a new branch for your feature or bugfix.
2. Submit a pull request with a detailed explanation of the changes.

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments
- Google Test for the testing framework.
- CMake for build automation.
- OpenSSL for cryptographic functionality.

