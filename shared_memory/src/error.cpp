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
