add_executable(fsf main.cpp)

target_include_directories(fsf PRIVATE
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(fsf 
    PRIVATE 
    OpenSSL::Crypto
)
