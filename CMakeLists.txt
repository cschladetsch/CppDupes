cmake_minimum_required(VERSION 3.10)

# Project setup
project(fsf)

# Enable C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add common dependencies
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
find_package(OpenSSL REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Include subdirectories
add_subdirectory(src)
add_subdirectory(tests)

