# Add test sources
file(GLOB TEST_SOURCES *.cpp)

# Add test executable
add_executable(fsf_tests ${TEST_SOURCES} ../src/DirectoryComparer.cpp ../src/FileHashMapper.cpp)

# Link libraries required by the test executable
target_link_libraries(fsf_tests GTest::GTest GTest::Main pthread OpenSSL::Crypto Boost::program_options)

# Set the output directory for the test executable
set_target_properties(fsf_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Register the test suite with CTest
add_test(NAME FileHashMapperTests COMMAND ${CMAKE_BINARY_DIR}/bin/fsf_tests)

