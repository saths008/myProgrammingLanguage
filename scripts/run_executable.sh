#!/bin/bash

#Run the app from the build directory
BUILD_DIR="../build/"
cd "$BUILD_DIR"
echo "Running make..."
make
./MyProgrammingLanguage_App "$input_args"

