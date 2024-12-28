#!/bin/bash

# Enable strict error checking
set -euo pipefail
IFS=$'\n\t'

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Default settings
VERBOSE=0
TEST_FILTER=""
CLEAN=1
BUILD=1
TEST=1

log() {
    echo -e "${BOLD}[$1]${NC} ${2}"
}

# Clean step
log "CLEAN" "Cleaning build directory..."
rm -rf build
mkdir -p build

# Build step
log "BUILD" "Configuring project..."
cmake -S . -B build

log "BUILD" "Building project..."
cmake --build build

# List generated files
log "INFO" "${BLUE}Generated files:${NC}"
echo -e "${YELLOW}Build outputs:${NC}"
find build -type f -executable -printf "  %p\n"

echo -e "\n${YELLOW}Installed binaries:${NC}"
ls -l ~/bin/fsf ~/bin/fsf_tests

# Verify binaries
if [ ! -f ~/bin/fsf ]; then
    log "ERROR" "${RED}fsf binary not found in ~/bin${NC}"
    exit 1
fi

if [ ! -f ~/bin/fsf_tests ]; then
    log "ERROR" "${RED}fsf_tests binary not found in ~/bin${NC}"
    exit 1
fi

# Test step
log "TEST" "Running tests..."
~/bin/fsf_tests

log "SUCCESS" "${GREEN}Build and tests completed successfully${NC}"
