cmake_minimum_required(VERSION 3.31.6)

enable_testing()

add_executable(test_shared_memory
    test_shared_memory.cpp
    test_error.cpp
    test_owned_fd.cpp
)

# Include the private header files
target_include_directories(test_shared_memory
    PRIVATE ${CMAKE_SOURCE_DIR}/shared_memory/src
)

target_link_libraries(test_shared_memory
    shared_memory
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_shared_memory)