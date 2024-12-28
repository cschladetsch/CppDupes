cmake_minimum_required(VERSION 3.10)
project(CppDupes)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL
find_package(OpenSSL REQUIRED)
set(OPENSSL_CRYPTO_LIB OpenSSL::Crypto)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Add include directories
include_directories(include)

# Subdirectories for source and tests
add_subdirectory(src)
add_subdirectory(tests)

# Check and fetch rang.hpp if not present
set(RANG_PATH "${CMAKE_SOURCE_DIR}/include/rang.hpp")
if (NOT EXISTS ${RANG_PATH})
    message(STATUS "Downloading rang.hpp...")
    file(DOWNLOAD https://raw.githubusercontent.com/agauniyal/rang/master/include/rang.hpp
         ${RANG_PATH})
endif()
