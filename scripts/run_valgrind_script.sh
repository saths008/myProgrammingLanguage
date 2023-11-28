#!/bin/bash
./build_script.sh
BUILD_DIR="../build/"
cd $BUILD_DIR
echo "Running valgrind..."
make memLeak
