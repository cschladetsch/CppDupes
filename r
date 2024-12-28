#!/bin/bash

set -e

# Clean build directory
rm -rf build
mkdir -p build

# Build the project
cmake -S . -B build
cmake --build build

# Check if binaries exist in ~/bin
if [ -f ~/bin/fsf ]; then
    echo "fsf is in ~/bin"
else
    echo "Error: fsf binary not found."
    exit 1
fi

if [ -f ~/bin/fsf_tests ]; then
    echo "fsf_tests is in ~/bin"
    # Run the tests
    ~/bin/fsf_tests
else
    echo "Error: fsf_tests binary not found."
    exit 1
fi
