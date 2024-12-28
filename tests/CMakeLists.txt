# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Enable testing
enable_testing()

# Add test sources
file(GLOB TEST_SOURCES *.cpp)

# Add the test executable
add_executable(fsf_tests ${TEST_SOURCES})

# Link against the Google Test and other required libraries
target_link_libraries(fsf_tests GTest::GTest GTest::Main pthread OpenSSL::Crypto)

# Set output directory for the test executable
set_target_properties(fsf_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Register the test suite with CTest
add_test(NAME FileHashMapperTests COMMAND ${CMAKE_BINARY_DIR}/bin/fsf_tests)

