# shared-memory

A C++23 POSIX shared memory library for Linux.

## Platform Support

**Linux only.** This library uses Linux-specific APIs (`shm_open`, `mmap`, etc.) and will not compile or configure on other platforms. A compile-time `#error` is emitted on non-Linux, and CMake configuration fails with a clear message.

## Using as a Dependency

Add this project via FetchContent or as a git submodule.

### FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(shared_memory
    GIT_REPOSITORY https://github.com/your-org/shared-memory.git
    GIT_TAG        v0.0.1
)
FetchContent_MakeAvailable(shared_memory)

target_link_libraries(myapp PRIVATE shared_memory)
```

### Git Submodule

```bash
git submodule add https://github.com/your-org/shared-memory.git external/shared-memory
```

```cmake
add_subdirectory(external/shared-memory)

target_link_libraries(myapp PRIVATE shared_memory)
```

## Building (Standalone)

```bash
cmake -B build -S .
cmake --build build
```

To build with tests:

```bash
cmake -B build -S . -DBUILD_TESTS=ON
cmake --build build
./build/tests/test_shared_memory
```
