#include "shared_memory/shared_memory.hpp"

#include <sys/mman.h>
#include <utility>

#include "owned_fd.hpp"

namespace shared_memory {

namespace {

static constexpr int
to_prot(const access_mode mode) noexcept
{
    switch (mode) {
        case access_mode::READ:         return PROT_READ;
        case access_mode::WRITE:        return PROT_WRITE;
        case access_mode::READ_WRITE:   return PROT_READ | PROT_WRITE;
    }
    return PROT_READ | PROT_WRITE;
}

}

[[nodiscard]] std::expected<shared_memory, error>
shared_memory::create(std::string shm_name, const std::size_t size, access_mode mode, const bool should_unlink) noexcept 
{
    auto shm_fd = owned_fd(shm_open(shm_name.c_str(), O_RDWR | O_CREAT | O_EXCL, std::to_underlying(mode)));
    if (!shm_fd.is_valid()) {
        return std::unexpected(error(errc::open_failed, {errno, std::generic_category()}));
    }

    if (ftruncate(shm_fd.get(), static_cast<off_t>(size)) == -1) {
        auto err = error(errc::truncate_failed, {errno, std::generic_category()});
        shm_unlink(shm_name.c_str());
        return std::unexpected(err);
    }

    void *addr = mmap(nullptr, size, to_prot(mode), MAP_SHARED, shm_fd.get(), 0);
    if (addr == MAP_FAILED) {
        auto err = error(errc::map_failed, {errno, std::generic_category()});
        shm_unlink(shm_name.c_str());
        return std::unexpected(err);
    }

    return shared_memory(std::move(shm_name), {static_cast<std::byte *>(addr), size}, should_unlink);
}

[[nodiscard]] std::expected<shared_memory, error>
shared_memory::open(std::string shm_name) noexcept 
{
    auto shm_fd = owned_fd(shm_open(shm_name.c_str(), O_RDWR, S_IRUSR | S_IWUSR));
    if (!shm_fd.is_valid()) {
        return std::unexpected(error(errc::open_failed, {errno, std::generic_category()}));
    }

    struct stat st{};
    if (fstat(shm_fd.get(), &st) == -1) {
        return std::unexpected(error(errc::stat_failed, {errno, std::generic_category()}));
    }

    auto size = static_cast<std::size_t>(st.st_size);
    void *addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd.get(), 0);
    if (addr == MAP_FAILED) {
        return std::unexpected(error(errc::map_failed, {errno, std::generic_category()}));
    }

    return shared_memory(std::move(shm_name), {static_cast<std::byte *>(addr), size}, false);
}

} // namespace shared_memory
