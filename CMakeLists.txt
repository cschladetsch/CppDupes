cmake_minimum_required(VERSION 3.10)

# Project setup
project(fsf)

# Enable C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Find dependencies
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GTEST_INCLUDE_DIRS})

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Link Boost libraries globally
set(BOOST_LIBS Boost::program_options)

