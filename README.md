# myProgrammingLanguage

This project was made by following and adapting the book: "Crafting Interpreters" by Bob Nystrom, which can be found at: `https://craftinginterpreters.com/`.

# Set up

Please ensure:

- a version of clang that supports c++17 is installed.
- atleast cmake 3.22 is installed.

1. Create a folder named third_party and clone google test into this folder.

googletest can be found at: `https://github.com/google/googletest`

## Run the compiler

### Run on MacOS/Linux

1.  Go to the scripts folder and execute the ./build_script.sh.
2.  Navigate back to the root and to run the executable, run './build/MyProgrammingLanguage_App' or to run an example program run './build/MyProgrammingLanguage_App example_programs/fib.vi'.

### Run on other systems

- If that doesn't work or you are on Windows, then:

1.  Create and navigate to the build folder in the root.
2.  run 'cmake ..' (ie. put the output of cmake into the build folder)
3.  run 'make'
4.  Navigate back to the root and to run the executable, run './build/MyProgrammingLanguage_App' or to run an example program run './build/MyProgrammingLanguage_App example_programs/fib.vi'.
