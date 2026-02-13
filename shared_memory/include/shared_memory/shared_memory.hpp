#pragma once

#if !defined(__linux__)
#error "shared_memory is only supported on Linux"
#endif

#include <span>
#include <cstddef>
#include <string>
#include <expected>
#include <utility>
#include <algorithm>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "shared_memory/error.hpp"

namespace shared_memory {

/**
 * @brief Access mode for a newly created shared memory segment.
 *
 * Determines both the file permissions (shm_open) and the mmap protection flags.
 */
enum class access_mode : mode_t {
    READ = S_IRUSR,
    WRITE = S_IWUSR,
    READ_WRITE = S_IRUSR | S_IWUSR
};

/**
 * @brief RAII wrapper for POSIX shared memory.
 *
 * Manages the lifetime of a shared memory mapping, including creation (owning)
 * or attachment (non-owning), and optionally unlinking the segment on destruction.
 * Non-copyable but supports move semantics.
 */
class shared_memory {
public:
    /** @brief Constructs an empty shared_memory object with no mapping. */
    shared_memory() noexcept
    : _name(""),
      _mem_view({}),
      _should_unlink(false)
    {}

    /** @brief Destructor. Unmaps memory and optionally unlinks the segment if owning. */
    ~shared_memory() { _close_shm(); }

    /* Non-copyable */
    shared_memory(const shared_memory&) = delete;
    shared_memory& operator=(const shared_memory&) = delete;

    /** @brief Move constructor. Transfers ownership of the mapping from the source. */
    shared_memory(shared_memory&& other) noexcept
    : _name(std::move(other._name)),
      _mem_view(std::exchange(other._mem_view, {})),
      _should_unlink(std::exchange(other._should_unlink, false))
    {}

    /** @brief Move assignment. Unmaps current mapping and takes ownership from @p other. */
    shared_memory& operator=(shared_memory&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        _close_shm();

        _name = std::move(other._name);
        _mem_view = std::exchange(other._mem_view, {});
        _should_unlink = std::exchange(other._should_unlink, false);

        return *this;
    }

    /**
     * @brief Creates a new shared memory segment and takes ownership.
     * @param shm_name The name of the segment (leading slash is optional).
     * @param size The size of the segment in bytes.
     * @param mode Access mode for the segment (default: read-write).
     * @param should_unlink If true, unlinks the segment on destruction (default: true).
     * @return The shared_memory object, or an error on failure.
     */
    [[nodiscard]] static std::expected<shared_memory, error>
    create(std::string shm_name, const std::size_t size, access_mode mode = access_mode::READ_WRITE, const bool should_unlink = true) noexcept;

    /**
     * @brief Opens an existing shared memory segment (non-owning).
     * @param shm_name The name of the segment to attach to.
     * @return The shared_memory object, or an error on failure.
     */
    [[nodiscard]] static std::expected<shared_memory, error>
    open(std::string shm_name) noexcept;

    /**
     * @brief Returns the size of the mapped memory region in bytes.
     * @return The number of bytes in the shared memory mapping.
     */
    [[nodiscard]] std::size_t 
    size() const noexcept { return _mem_view.size(); }

    /**
     * @brief Checks whether this object has an active mapping.
     * @return true if the mapping is empty (e.g., default-constructed), false otherwise.
     */
    [[nodiscard]] bool
    empty() const noexcept { return _mem_view.empty(); }

    /**
     * @brief Returns the full mapped memory region as a mutable span.
     * @return A span over the entire shared memory.
     */
    [[nodiscard]] std::span<std::byte> get_memory() noexcept { return _mem_view; }

    /**
     * @brief Returns the full mapped memory region as a read-only span.
     * @return A const span over the entire shared memory.
     */
    [[nodiscard]] std::span<const std::byte> 
    get_memory() const noexcept { return _mem_view; }

    /**
     * @brief Writes data into the shared memory at the given offset.
     * @param offset Byte offset at which to write.
     * @param data Data to copy into the shared memory.
     * @return true if the write succeeded, false if offset or size is out of bounds.
     */
    [[nodiscard]] bool
    write(const std::size_t offset, const std::span<const std::byte> data) noexcept
    {
        if (!_is_bounds_valid(offset, data.size())) [[unlikely]] {
            return false;
        }

        std::ranges::copy(data, _mem_view.begin() + offset);

        return true;
    }

    /**
     * @brief Returns a mutable subspan of the shared memory.
     * @param offset Starting byte offset.
     * @param count Number of bytes in the subspan.
     * @return A span over [offset, offset+count), or empty span if out of bounds.
     */
    [[nodiscard]] std::span<std::byte>
    view(const std::size_t offset, const std::size_t count) noexcept
    {
        if (!_is_bounds_valid(offset, count)) [[unlikely]] {
            return {};
        }

        return _mem_view.subspan(offset, count);
    }

    /**
     * @brief Returns a read-only subspan of the shared memory.
     * @param offset Starting byte offset.
     * @param count Number of bytes in the subspan.
     * @return A const span over [offset, offset+count), or empty span if out of bounds.
     */
    [[nodiscard]] std::span<const std::byte>
    view(const std::size_t offset, const std::size_t count) const noexcept
    {
        if (!_is_bounds_valid(offset, count)) [[unlikely]] {
            return {};
        }

        return _mem_view.subspan(offset, count);
    }

private:

    explicit shared_memory(std::string name, std::span<std::byte> mem_view, bool should_unlink) noexcept
    : _name(std::move(name)),
      _mem_view(mem_view),
      _should_unlink(should_unlink)
    {}

    [[nodiscard]] bool 
    _is_bounds_valid(const std::size_t offset, const std::size_t count) const noexcept
    {
        return count <= _mem_view.size() && offset <= (_mem_view.size() - count);
    }

    void 
    _close_shm() noexcept 
    {
        if (!_mem_view.empty()) {
            munmap(_mem_view.data(), _mem_view.size());
        }

        if (_should_unlink) {
            shm_unlink(_name.c_str());
        }
    }

private:
    std::string _name{};
    std::span<std::byte> _mem_view{};
    bool _should_unlink{false};
};

} // namespace shared_memory
