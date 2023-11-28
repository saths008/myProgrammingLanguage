#!/bin/bash
echo "Running Test Script..."

BUILD_DIR="../build"
cd $BUILD_DIR
echo "Running make..."
make

TEST_DIR="../build/test"
cd $TEST_DIR
echo "Running Tests..."
./Tests
echo "Test Script Complete"
