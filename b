#!/bin/bash

BUILD_DIR="build"
BIN_DIR="$BUILD_DIR/bin"

total_clean() {
    rm -rf $BUILD_DIR
    echo "Build directory cleaned."
}

build_project() {
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR || exit
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    cd ..
}

run_tests() {
    local verbose=$1
    cd $BUILD_DIR || exit
    if [ ! -f "bin/fsf_test" ]; then
        echo "Error: Test executable not found at bin/fsf_test"
        exit 1
    fi
    
    if [ "$verbose" = true ]; then
        ctest --output-on-failure -V
    else
        ctest --output-on-failure
    fi
    cd ..
}

show_help() {
    echo "Usage: b [options]"
    echo "  -c    Clean build"
    echo "  -t    Build and test"
    echo "  -v    Verbose test output"
    echo "  -ct   Clean, build, and test"
    echo "  -a    All: Clean, build, and test with verbose output"
}

verbose=false

if [[ $# -eq 0 ]]; then
    build_project
else
    while getopts "ctva" opt; do
        case $opt in
            c) total_clean ;;
            t) build_project; run_tests $verbose ;;
            v) verbose=true ;;
            a) total_clean; verbose=true; build_project; run_tests $verbose ;;
            *) show_help; exit 1 ;;
        esac
    done

    if [[ $* == *-c* && $* == *-t* ]]; then
        total_clean
        build_project
        run_tests $verbose
    fi
fi
