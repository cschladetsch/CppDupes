#!/bin/bash

BUILD_DIR="build"
BIN_DIR="$BUILD_DIR/bin"

# Ensure ~/bin exists
mkdir -p ~/bin

# Functions for tasks
total_clean() {
    rm -rf $BUILD_DIR
    echo "Build directory cleaned."
}

build_project() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR || exit 1
    cmake .. || { echo "CMake configuration failed."; exit 1; }
    make || { echo "Build process failed."; exit 1; }
    cd ..
    echo "Build completed successfully."

    # Move the main executable to ~/bin
    if [ -f "$BIN_DIR/fsf" ]; then
        mv "$BIN_DIR/fsf" ~/bin/fsf || { echo "Failed to move executable to ~/bin."; exit 1; }
        echo "Executable moved to ~/bin."
    else
        echo "Error: Executable not found."
        exit 1
    fi
}

run_tests() {
    if [ -f "$BIN_DIR/fsf_tests" ]; then
        $BIN_DIR/fsf_tests
        echo "Tests executed successfully."
    else
        echo "Error: Test executable not found."
        exit 1
    fi
}

# Parse combined options
tasks=$(echo "$1" | sed 's/-//g')
for ((i = 0; i < ${#tasks}; i++)); do
    case "${tasks:$i:1}" in
        c) total_clean ;;
        b) build_project ;;
        t) run_tests ;;
        *)
            echo "Unknown option: ${tasks:$i:1}"
            echo "Usage: $0 [-c] [-b] [-t]"
            exit 1
            ;;
    esac
done

