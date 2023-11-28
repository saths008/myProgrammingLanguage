cmake_minimum_required(VERSION 3.22)
#Set up C++ compiler and flags
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(This MyProgrammingLanguage)
project(${This} C CXX)
# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
enable_testing()
# Find and include Google Test
add_subdirectory(third_party/googletest)
include_directories(src)
set (Headers 
  src/expr.hpp
  src/parser.hpp
  src/scanner.hpp
  src/token.hpp
  src/tokentype.hpp
  )

set (Sources 
  src/expr.cpp
  src/parser.cpp
  src/scanner.cpp
  src/token.cpp
  src/main.cpp
  )
add_library(${This} STATIC ${Sources} ${Headers})
add_subdirectory(test)
add_executable(${This}_App src/main.cpp)
target_link_libraries(${This}_App PRIVATE ${This})
set(VALGRIND_CMD valgrind --leak-check=yes --log-file=valgrind.rpt)
add_custom_target(memLeak
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/valgrind_script.sh $<TARGET_FILE:${This}_App>
    COMMAND ${CMAKE_COMMAND} -E echo "Running with valgrind..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E echo "valgrind report generated."

)

# Add custom target for debug
add_custom_target(debug
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${This}_App --config Debug
)

