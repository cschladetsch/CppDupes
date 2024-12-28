# Create library from source files
add_library(fsf_lib
    FileHashMapper.cpp
    DirectoryComparer.cpp
)

# Link OpenSSL to the library
target_link_libraries(fsf_lib PUBLIC OpenSSL::Crypto)

# Create main executable
add_executable(fsf_exec main.cpp)

# Link our library to the executable
target_link_libraries(fsf_exec PRIVATE fsf_lib)
