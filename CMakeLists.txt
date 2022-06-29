cmake_minimum_required(VERSION 3.0)

project(quickjs-bind)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILE_FEATURES "${CMAKE_CXX_COMPILE_FEATURES} /MT /GR- /utf-8")
add_definitions(-DUNICODE -D_UNICODE)

include_directories(quickjs-cmake)

# build quickjs static library
add_subdirectory(quickjs-cmake)

# build test program
add_executable(test-bind test/main.cc)
target_link_libraries(test-bind quickjs)