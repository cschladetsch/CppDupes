add_executable(fsf main.cpp DirectoryComparer.cpp FileHashMapper.cpp)
target_link_libraries(fsf PRIVATE ${OPENSSL_CRYPTO_LIB})


