#!/bin/bash

#Run valgrind on the app from the build directory
DEFAULT_FILEPATH="src/hello.saath"

if [ -t 0 ]; then
    echo "Do you want to use the default file path: $DEFAULT_FILEPATH? (y/n)"
    read use_default

    if [ "$use_default" == "y" ]; then
        input_args="$DEFAULT_FILEPATH"
    else
        echo "Enter file: "
        read input_args
    fi
else
    input_args="$DEFAULT_FILEPATH"
fi
echo valgrind --leak-check=yes --log-file=valgrind.rpt "$@" "$input_args"
valgrind --leak-check=yes --log-file=valgrind.rpt "$@" "$input_args"
