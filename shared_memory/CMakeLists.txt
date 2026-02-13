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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT shared_memoryTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT shared_memoryTargets
    FILE shared_memoryTargets.cmake
    NAMESPACE shared_memory::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shared_memory
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/shared_memoryConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/shared_memoryConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shared_memory
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/shared_memoryConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/shared_memoryConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/shared_memoryConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/shared_memory
)