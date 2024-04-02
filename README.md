# myProgrammingLanguage

This project was made by following the book: "Crafting Interpreters" by Bob Nystrom, which can be found at: `https://craftinginterpreters.com/`.

My own interpreted programming language.

# Set up

Please ensure:

- a version of clang that supports c++17 is installed.
- atleast cmake 3.22 is installed.

1. Create a folder named third_party and clone google test into this folder.

googletest can be found at: `https://github.com/google/googletest`

2. Go to the scripts folder and execute the ./build_script.sh.

- If that doesn't work or you are on Windows, then:

1.  Create and navigate to the build folder in the root.
2.  run cmake .. (ie. put the output of cmake into the build folder)
3.  run make
4.  run the executable ./build/MyProgrammingLanguage_App

# High-Level Overview

We have our program translated into bytecode, which is then executed by the VM. Bytecode allows us to target some idealised imaginary instruction set and so requiring emulation (software pretending to a be a chip in order to execute the bytecode, one instruction at a time)
