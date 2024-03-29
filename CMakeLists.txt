cmake_minimum_required(VERSION 3.22)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")
set(This MyProgrammingLanguage)
project(${This} C CXX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
enable_testing()
# Find and include Google Test
add_subdirectory(third_party/googletest)
include_directories(src)
set (Headers 
  src/bytecodeSeq.h
  src/common.h
  src/constantPoolArray.h
  src/virtualMachine.h
  src/value.h
  src/object.h
  src/hashTable.h
src/scanner.h
src/debug.h
src/table.h
src/memory.h
src/compiler.h
)

set (Sources 
  src/bytecodeSeq.c
  src/constantPoolArray.c
  src/debug.c
  src/memory.c
  src/value.c
src/table.c

  src/object.c
src/scanner.c
src/compiler.c
  src/hashTable.c
  src/virtualMachine.c
  src/main.c)
add_library(${This} STATIC ${Sources} ${Headers})
add_subdirectory(test)
add_executable(${This}_App src/main.c)
target_link_libraries(${This}_App PRIVATE ${This})
set(VALGRIND_CMD valgrind --leak-check=yes --log-file=valgrind.rpt)
add_custom_target(memLeak
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/valgrind_script.sh $<TARGET_FILE:${This}_App>
    COMMAND ${CMAKE_COMMAND} -E echo "Running with valgrind..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "valgrind report generated."
    DEPENDS ${This}_App  # Ensure that the executable is built before running the target
)

# Add custom target for debug
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${This}_App --config Debug
)
