/**************************************************************
 * @file error.cpp
 * @brief Implementation of error::message() for shared memory
 * error reporting.
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

#include "shared_memory/error.hpp"

#include <format>
#include <string_view>

namespace shared_memory {

namespace {

[[nodiscard]] constexpr std::string_view
to_string(errc kind) noexcept
{
    switch (kind) {
        case errc::open_failed:     return "shared memory open failed";
        case errc::truncate_failed: return "shared memory truncate failed";
        case errc::map_failed:      return "shared memory map failed";
        case errc::stat_failed:     return "shared memory stat failed";
        default:                    return "unknown shared memory error";
    }
}

}

std::string
error::message() const
{
    return std::format("{}: {}", to_string(_kind), _code.message());
}

} // namespace shared_memory
