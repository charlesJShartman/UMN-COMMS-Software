cmake_minimum_required(VERSION 3.10)

# Set project name and version
project(COMMS VERSION 1.0)
add_compile_options(-Wall -Wextra -Werror -Wpedantic)

add_compile_options(-g)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

enable_testing()

include_directories(include)
include_directories(common/include)

add_subdirectory(common)
add_subdirectory(src)
add_subdirectory(tests)
