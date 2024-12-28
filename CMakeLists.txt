cmake_minimum_required(VERSION 3.10)

# Project setup
project(fsf)

# Enable C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add OpenSSL
find_package(OpenSSL REQUIRED)

# Add main executable
add_executable(fsf_main src/main.cpp)
target_link_libraries(fsf_main OpenSSL::Crypto)

# Add test sources
file(GLOB TEST_SOURCES tests/*.cpp)
add_executable(fsf_tests ${TEST_SOURCES})
target_link_libraries(fsf_tests ${GTEST_LIBRARIES} pthread OpenSSL::Crypto)

# Set output directories
set_target_properties(fsf_main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(fsf_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add tests to CTest
add_test(NAME GoogleTest COMMAND ${CMAKE_BINARY_DIR}/bin/fsf_tests)
