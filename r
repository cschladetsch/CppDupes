#!/bin/bash

# Check if the build script exists
if [ ! -f "./b" ]; then
    echo "Error: Build script './b' not found."
    exit 1
fi

# Ensure the build script is executable
chmod +x ./b

# Execute the build script with the desired options
./b -cbt

