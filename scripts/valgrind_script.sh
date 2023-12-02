#!/bin/bash

echo valgrind --leak-check=yes --log-file=valgrind.rpt "$@" 
valgrind --leak-check=yes --log-file=valgrind.rpt "$@" 
