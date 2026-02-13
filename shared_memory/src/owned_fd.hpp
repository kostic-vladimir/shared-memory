/**************************************************************
 * @file owned_fd.hpp
 * @brief RAII wrapper for owning a file descriptor used by
 * shared memory operations.
 **************************************************************/

/**************************************************************
 * Copyright (c) 2026 Vladimir Kostic
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of shared_memory
 *
 * Author:          Vladimir Kostic <vladimir.kostic1997@gmail.com>
 * Version:         v1.0.0
 **************************************************************/

#pragma once

#include <utility>
#include <errno.h>
#include <unistd.h>

namespace shared_memory {

/**
 * @brief RAII wrapper for owning a file descriptor.
 *
 * Manages the lifecycle of a file descriptor, ensuring it is closed on destruction.
 * Non-copyable but supports move semantics.
 */
class owned_fd {
public:
    /** @brief Sentinel value indicating an invalid or unowned file descriptor. */
    static constexpr int INVALID_FD{-1};

    /**
     * @brief Constructs an owned_fd taking ownership of the given file descriptor.
     * @param fd The file descriptor to own. Use INVALID_FD for no ownership.
     */
    explicit owned_fd(const int fd)
    : _fd(fd)
    {}

    /** @brief Default constructor. Creates an owned_fd with no file descriptor. */
    owned_fd() = default;

    /** @brief Destructor. Closes the owned file descriptor if valid. */
    ~owned_fd() { _reset(); }

    /* Non-copyable */
    owned_fd(const owned_fd &) noexcept = delete;
    owned_fd& operator=(const owned_fd &) noexcept = delete;

    /**
     * @brief Move constructor. Transfers ownership from @p other.
     * @param other The owned_fd to move from. Becomes invalid after the move.
     */
    owned_fd(owned_fd &&other) noexcept
    : _fd(std::exchange(other._fd, INVALID_FD))
    {}

    /**
     * @brief Move assignment operator. Closes current fd and takes ownership from @p other.
     * @param other The owned_fd to move from. Becomes invalid after the move.
     * @return Reference to *this.
     */
    owned_fd& operator=(owned_fd &&other) noexcept
    {
        if (this != &other) {
            _reset();
            _fd = std::exchange(other._fd, INVALID_FD);
        }
        return *this;
    }

    /**
     * @brief Returns the underlying file descriptor without transferring ownership.
     * @return The file descriptor, or INVALID_FD if none is owned.
     */
    [[nodiscard]] int
    get() const noexcept { return _fd; }

    /**
     * @brief Checks whether this object owns a valid file descriptor.
     * @return true if the file descriptor is valid (>= 0), false otherwise.
     */
    [[nodiscard]] bool
    is_valid() const noexcept { return _fd >= 0; }

private:
    void
    _reset() noexcept
    {
        if (_fd >= 0) {
            const int saved_errno{errno};
            ::close(_fd);
            errno = saved_errno;
            _fd = -1;
        }
    }

private:
    int _fd{INVALID_FD};
};

} // namespace shared_memory
