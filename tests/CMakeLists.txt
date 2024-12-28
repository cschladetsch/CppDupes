# Add test executable
add_executable(fsf_tests
    FileHashMapperTests.cpp
    DirectoryComparatorTests.cpp
    tests.cpp
)

# Link against GTest and our library
target_link_libraries(fsf_tests 
    PRIVATE 
        fsf_lib
        GTest::GTest 
        GTest::Main
        ${CMAKE_THREAD_LIBS_INIT}
)

# Add a custom target to list all tests
add_custom_target(list_tests
    COMMAND $<TARGET_FILE:fsf_tests> --gtest_list_tests
    DEPENDS fsf_tests
)

# Discover and add tests
gtest_discover_tests(fsf_tests)
