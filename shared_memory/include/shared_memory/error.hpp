/**************************************************************
 * @file error.hpp
 * @brief Error types and handling for POSIX shared memory
 * operations on Linux.
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

#if !defined(__linux__)
#error "shared_memory is only supported on Linux"
#endif

#include <string>
#include <system_error>

namespace shared_memory {

/**
 * @brief Enumerates the shared memory operations that can fail.
 *
 * Each enumerator corresponds to a specific POSIX call that may
 * produce a system error during the lifetime of a shared_memory object.
 */
enum class errc {
    open_failed,
    truncate_failed,
    map_failed,
    stat_failed
};

/**
 * @brief Represents a failed shared memory operation paired with a system error code.
 *
 * Combines a domain-specific error kind (errc) with the underlying POSIX
 * error (std::error_code), providing both programmatic inspection and
 * human-readable formatting.
 */
class error {
public:
    /**
     * @brief Constructs an error from an operation kind and a system error code.
     * @param kind Which shared memory operation failed.
     * @param code The underlying system error code (typically from errno).
     */
    error(errc kind, std::error_code code) noexcept
    : _kind(kind), _code(code)
    {}

    /**
     * @brief Returns which shared memory operation failed.
     * @return The errc value identifying the failed operation.
     */
    [[nodiscard]] errc
    kind() const noexcept { return _kind; }

    /**
     * @brief Returns the underlying system error code.
     * @return A const reference to the std::error_code.
     */
    [[nodiscard]] const std::error_code&
    code() const noexcept { return _code; }

    /**
     * @brief Returns a formatted message combining the operation and system error.
     * @return A string like "shared memory open failed: Permission denied".
     */
    [[nodiscard]] std::string
    message() const;

    /** @brief Equality comparison. */
    bool operator==(const error&) const = default;

private:
    errc _kind;
    std::error_code _code;
};

} // namespace shared_memory
