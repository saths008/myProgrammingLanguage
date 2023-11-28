#!/bin/bash

#Run the app from the build directory
DEFAULT_FILEPATH="../src/hello.saath"
BUILD_DIR="../build/"

echo "Do you want to use the default file path: $DEFAULT_FILEPATH? (y/n)"
read use_default

if [ "$use_default" == "y" ]; then
    input_args="$DEFAULT_FILEPATH"
else
    echo "Enter file: "
    read input_args
fi

cd "$BUILD_DIR"
echo "Running make..."
make
./MyProgrammingLanguage_App "$input_args"

