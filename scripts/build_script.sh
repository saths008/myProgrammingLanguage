#!/bin/bash

#Remove the build dir, create a new one, run cmake, run make


BUILD_DIR="../build/"

# Check if the build directory exists
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -r "$BUILD_DIR"
fi

# Create a new build directory
echo "Creating a new build directory..."
mkdir "$BUILD_DIR"

# Change to the build directory
cd "$BUILD_DIR"

# Run cmake
echo "Running cmake..."
cmake ..
echo "Running make..."
make
echo "Build script completed."



