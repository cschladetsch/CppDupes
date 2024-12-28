#!/bin/bash

BUILD_DIR="build"
BIN_DIR="$BUILD_DIR/bin"

# Function to clean the build directory
total_clean() {
    rm -rf $BUILD_DIR
    echo "Build directory cleaned."
}

# Function to build the project
build_project() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR || exit 1
    cmake -DCMAKE_BUILD_TYPE=Release .. || { echo "CMake configuration failed."; exit 1; }
    make || { echo "Build process failed."; exit 1; }
    cd ..
    echo "Build completed successfully."
}

# Function to run tests
run_tests() {
    if [ -f "$BIN_DIR/fsf_tests" ]; then
        $BIN_DIR/fsf_tests
        echo "Tests executed successfully."
    elif [ -f "$BUILD_DIR/CTestTestfile.cmake" ]; then
        cd $BUILD_DIR || exit 1
        ctest --output-on-failure
        cd ..
        echo "CTest executed successfully."
    else
        echo "Error: No test executable or CTest configuration found."
        exit 1
    fi
}

# Parse command line arguments
while getopts "cbt" opt; do
    case $opt in
        c)
            total_clean
            ;;
        b)
            build_project
            ;;
        t)
            total_clean
            build_project
            run_tests
            ;;
        ?)
            echo "Usage: $0 [-c] [-b] [-t]"
            exit 1
            ;;
    esac
done

