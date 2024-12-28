#!/bin/bash

# Potential executable locations
EXECUTABLE="$HOME/bin/fsf"

# Check if executable exists
if [ ! -x "$EXECUTABLE" ]; then
    echo "Executable not found. Building project..."
    
    # Build steps
    mkdir -p build
    cd build
    cmake ..
    make
    
    # Install to ~/bin if it doesn't exist
    mkdir -p "$HOME/bin"
    cp bin/fsf "$HOME/bin/"
fi

# Run the executable with all provided arguments
"$EXECUTABLE" "$@"
