# Add source files for the main application
add_executable(fsf main.cpp DirectoryComparer.cpp FileHashMapper.cpp)

# Link libraries required by the application
target_link_libraries(fsf OpenSSL::Crypto Boost::program_options)

# Set the output directory for the executable
set_target_properties(fsf PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

