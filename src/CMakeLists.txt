# Add main executable
add_executable(fsf_main main.cpp FileHashMapper.cpp)

# Link libraries for the main application
target_link_libraries(fsf_main OpenSSL::Crypto)

# Set output directory for fsf_main
set_target_properties(fsf_main PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{HOME}/bin)

