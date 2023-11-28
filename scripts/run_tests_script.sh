#!/bin/bash
./build_script.sh
TEST_DIR="../build/test"
cd $TEST_DIR
echo "Running Tests..."
./Tests
echo "Test Script Complete"
