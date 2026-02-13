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
