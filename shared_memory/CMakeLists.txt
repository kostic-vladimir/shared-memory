cmake_minimum_required(VERSION 3.31.6)

add_library(${PROJECT_NAME} STATIC
    src/shared_memory.cpp
    src/error.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_sources(${PROJECT_NAME} PRIVATE
    src/owned_fd.hpp
)
