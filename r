#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# Variables to track status
BUILD_STATUS="Not Started"
TEST_STATUS="Not Started"
BUILD_TIME=0
TEST_TIME=0
ERROR_COUNT=0
WARNING_COUNT=0
FAILED_TESTS=0
PASSED_TESTS=0
TOTAL_TESTS=0

# Print section header
print_header() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Print success message
print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

# Print error message
print_error() {
    echo -e "${RED}✗ $1${NC}"
    ((ERROR_COUNT++))
}

# Print warning/info message
print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

# Generate status indicator
get_status_indicator() {
    local status=$1
    case $status in
        "Success")
            echo -e "${GREEN}✓${NC}"
            ;;
        "Failed")
            echo -e "${RED}✗${NC}"
            ;;
        "Not Started")
            echo -e "${YELLOW}○${NC}"
            ;;
        *)
            echo -e "${YELLOW}?${NC}"
            ;;
    esac
}

# Print summary table
print_summary() {
    local width=60
    local separator=$(printf '%*s' "$width" | tr ' ' '=')
    
    echo -e "\n${BOLD}${CYAN}Build and Test Summary${NC}"
    echo -e "${BLUE}$separator${NC}"
    
    # Build Status
    printf "${BOLD}%-20s${NC}" "Build Status:"
    printf "%-30s" "$(get_status_indicator "$BUILD_STATUS") $BUILD_STATUS"
    printf "%10s\n" "${BUILD_TIME}s"
    
    # Test Status
    printf "${BOLD}%-20s${NC}" "Test Status:"
    printf "%-30s" "$(get_status_indicator "$TEST_STATUS") $TEST_STATUS"
    printf "%10s\n" "${TEST_TIME}s"
    
    echo -e "${BLUE}$separator${NC}"
    
    # Test Details
    printf "${BOLD}%-20s${NC}" "Test Results:"
    printf "${GREEN}Passed: %-10s${NC}" "$PASSED_TESTS"
    printf "${RED}Failed: %-10s${NC}\n" "$FAILED_TESTS"
    
    # Build Details
    printf "${BOLD}%-20s${NC}" "Build Details:"
    printf "${RED}Errors: %-10s${NC}" "$ERROR_COUNT"
    printf "${YELLOW}Warnings: %-10s${NC}\n" "$WARNING_COUNT"
    
    echo -e "${BLUE}$separator${NC}"
    
    # Total time
    local total_time=$((BUILD_TIME + TEST_TIME))
    printf "${BOLD}%-20s${NC}" "Total Time:"
    printf "${CYAN}%s${NC}\n" "${total_time}s"
    
    echo -e "${BLUE}$separator${NC}\n"
    
    # Final status message
    if [ "$BUILD_STATUS" = "Success" ] && [ "$TEST_STATUS" = "Success" ]; then
        echo -e "${GREEN}${BOLD}✓ Build and tests completed successfully!${NC}"
    else
        echo -e "${RED}${BOLD}✗ Build or tests had issues. Check the logs above for details.${NC}"
    fi
}

# Error handling
set -e
trap 'print_error "Error occurred in script at line $LINENO. Command: $BASH_COMMAND"' ERR

# Start timestamp
BUILD_START_TIME=$(date +%s)

# Store original directory
ORIGINAL_DIR=$(pwd)

# Clean build directory
print_header "Cleaning build directory..."
if [ -d "build" ]; then
    rm -rf build
    print_success "Cleaned existing build directory"
else
    print_info "No existing build directory found"
fi
mkdir build
print_success "Created fresh build directory"

# Configure project
print_header "Configuring project..."
cd build
if cmake .. -DCMAKE_BUILD_TYPE=Debug 2> >(grep -i "warning" >&2); then
    print_success "Project configured"
else
    BUILD_STATUS="Failed"
    print_error "Configuration failed"
    cd "$ORIGINAL_DIR"
    exit 1
fi

# Build project
print_header "Building project..."
CPU_CORES=$(nproc)
print_info "Building using $CPU_CORES cores"

# Create temporary directory for logs
TEMP_DIR=$(mktemp -d)
BUILD_LOG="${TEMP_DIR}/build.log"

# Build with warnings captured
if VERBOSE=1 make -j$CPU_CORES 2>&1 | tee "${BUILD_LOG}"; then
    BUILD_STATUS="Success"
    print_success "Project built successfully"
    
    # Count warnings if log file exists
    if [ -f "${BUILD_LOG}" ]; then
        WARNING_COUNT=$(grep -ci "warning:" "${BUILD_LOG}" || true)
    fi
else
    BUILD_STATUS="Failed"
    print_error "Build failed"
    
    # Count warnings even if build failed
    if [ -f "${BUILD_LOG}" ]; then
        WARNING_COUNT=$(grep -ci "warning:" "${BUILD_LOG}" || true)
    fi
    
    # Cleanup
    rm -rf "${TEMP_DIR}"
    cd "$ORIGINAL_DIR"
    exit 1
fi

# Cleanup
rm -rf "${TEMP_DIR}"

BUILD_END_TIME=$(date +%s)
BUILD_TIME=$((BUILD_END_TIME - BUILD_START_TIME))

# Run tests with verbose output
print_header "Running tests..."

TEST_START_TIME=$(date +%s)

# Define test execution function
run_tests() {
    ./fsf_tests \
        --gtest_color=yes \
        --gtest_print_time=1 \
        --gtest_brief=0 \
        --gtest_show_internal_stack_frames \
        --gtest_catch_exceptions=0
}

# First list all tests
cd tests
print_info "Available tests:"
TOTAL_TESTS=$(./fsf_tests --gtest_list_tests | grep -c "^[^ ]" || true)
./fsf_tests --gtest_list_tests
echo ""

# Then run them
print_info "Executing tests:"
if TEST_OUTPUT=$(run_tests); then
    TEST_STATUS="Success"
    print_success "All tests passed!"
    PASSED_TESTS=$TOTAL_TESTS
else
    TEST_STATUS="Failed"
    print_error "Some tests failed!"
    FAILED_TESTS=$(echo "$TEST_OUTPUT" | grep -c "FAILED" || true)
    PASSED_TESTS=$((TOTAL_TESTS - FAILED_TESTS))
fi

cd "$ORIGINAL_DIR/build"

TEST_END_TIME=$(date +%s)
TEST_TIME=$((TEST_END_TIME - TEST_START_TIME))

# Return to original directory
cd "$ORIGINAL_DIR"

# Print final summary
print_summary

# Exit with appropriate status
if [ "$BUILD_STATUS" = "Success" ] && [ "$TEST_STATUS" = "Success" ]; then
    exit 0
else
    exit 1
fi
