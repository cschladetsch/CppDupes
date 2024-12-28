#!/bin/bash

# Script 'r': Build and run the project, then execute all tests

# Exit immediately if a command exits with a non-zero status
set -e

# Define build directory
BUILD_DIR="build"

# Create or clean the build directory
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
else
    rm -rf "$BUILD_DIR"/*
fi

# Navigate to the build directory
cd "$BUILD_DIR"

# Run CMake configuration and build
cmake ..
make -j$(nproc)

# Navigate back to the root directory
cd ..

# Run all tests (assuming the main test binary is 'fsf_tests')
if [ -f "build/bin/fsf_tests" ]; then
    echo "Running all tests using fsf_tests binary:"
    ./build/bin/fsf_tests
else
    echo "Error: fsf_tests binary not found in build/bin/"
    exit 1
fi

