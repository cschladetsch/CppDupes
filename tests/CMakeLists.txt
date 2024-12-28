add_executable(fsf_tests tests.cpp ../src/FileHashMapper.cpp)
target_link_libraries(fsf_tests PRIVATE gtest gtest_main pthread ${OPENSSL_CRYPTO_LIB})

