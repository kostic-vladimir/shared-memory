cmake_minimum_required(VERSION 3.31.6)

project(shared_memory
    VERSION 0.0.1
    DESCRIPTION "Shared memory library for Linux"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "shared_memory only supports Linux. Current system: ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(shared_memory)

option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
endif()