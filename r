#!/bin/bash

set -e

# Build the project
cmake -S . -B build
cmake --build build

# Copy the main executable to ~/bin
if [ -f "build/bin/fsf" ]; then
    cp build/bin/fsf ~/bin/fsf_main
    echo "fsf_main copied to ~/bin"
else
    echo "Error: fsf binary not found in build/bin/"
    exit 1
fi

# Find and run the test binary
fsf_tests_binary=$(find build -type f -name "fsf_tests" | head -n 1)
if [ -n "$fsf_tests_binary" ]; then
    echo "Running test binary: $fsf_tests_binary"
    "$fsf_tests_binary"
else
    echo "Error: fsf_tests binary not found."
    exit 1
fi

