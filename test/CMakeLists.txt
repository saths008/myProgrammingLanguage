cmake_minimum_required(VERSION 3.22)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(This Tests)
set(Sources 
  ScannerTest.cpp
  )

add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
  gtest_main
  MyProgrammingLanguage
  )

add_test(
  NAME ${This}
  COMMAND ${This}
  )
