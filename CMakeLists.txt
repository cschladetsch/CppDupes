cmake_minimum_required(VERSION 3.10)
project(CppDupes VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find OpenSSL with more specific configuration
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")

if(NOT TARGET OpenSSL::Crypto)
    add_library(OpenSSL::Crypto STATIC IMPORTED)
    set_target_properties(OpenSSL::Crypto PROPERTIES
        IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}")
endif()

# Add the source subdirectory which contains the main executable
add_subdirectory(src)

# Enable testing
enable_testing()

# Add GTest
find_package(GTest REQUIRED)
include(GoogleTest)

# Add threads for concurrent tests
find_package(Threads REQUIRED)

# Add the tests subdirectory
add_subdirectory(tests)
