#include <gtest/gtest.h>

#include "shared_memory/shared_memory.hpp"

#include <array>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

namespace {

using shm_type = shared_memory::shared_memory;

std::string unique_shm_name() {
    static int counter = 0;
    return "/shm_test_" + std::to_string(getpid()) + "_" + std::to_string(counter++);
}

TEST(SharedMemoryTest, DefaultConstruction) {
    shm_type shm;
    EXPECT_TRUE(shm.empty());
    EXPECT_EQ(shm.size(), 0u);
    EXPECT_TRUE(shm.get_memory().empty());
}

TEST(SharedMemoryTest, CreateSucceeds) {
    auto result = shm_type::create(unique_shm_name(), 4096);
    ASSERT_TRUE(result.has_value());

    auto& shm = *result;
    EXPECT_FALSE(shm.empty());
    EXPECT_EQ(shm.size(), 4096u);
    EXPECT_EQ(shm.get_memory().size(), 4096u);
}

TEST(SharedMemoryTest, CreateAndOpenRoundTrip) {
    const std::string name = unique_shm_name();
    {
        auto create_result = shm_type::create(name, 1024, shared_memory::access_mode::READ_WRITE, false);
        ASSERT_TRUE(create_result.has_value());
        auto& creator = *create_result;
        ASSERT_TRUE(creator.write(0, std::span<const std::byte>(reinterpret_cast<const std::byte*>("hello"), 5)));
    }

    auto open_result = shm_type::open(name);
    ASSERT_TRUE(open_result.has_value());

    auto& reader = *open_result;
    EXPECT_EQ(reader.size(), 1024u);

    auto data = reader.get_memory().subspan(0, 5);
    EXPECT_EQ(0, std::memcmp(data.data(), "hello", 5));

    shm_unlink(name.c_str());
}

TEST(SharedMemoryTest, WriteAndRead) {
    auto result = shm_type::create(unique_shm_name(), 256);
    ASSERT_TRUE(result.has_value());

    auto& shm = *result;
    const std::array<std::byte, 4> in{std::byte{0xAB}, std::byte{0xCD}, std::byte{0xEF}, std::byte{0x12}};

    EXPECT_TRUE(shm.write(0, in));
    EXPECT_TRUE(shm.write(100, in));

    auto mem = shm.get_memory();
    EXPECT_EQ(mem[0], std::byte{0xAB});
    EXPECT_EQ(mem[1], std::byte{0xCD});
    EXPECT_EQ(mem[2], std::byte{0xEF});
    EXPECT_EQ(mem[3], std::byte{0x12});
    EXPECT_EQ(mem[100], std::byte{0xAB});
    EXPECT_EQ(mem[101], std::byte{0xCD});
    EXPECT_EQ(mem[102], std::byte{0xEF});
    EXPECT_EQ(mem[103], std::byte{0x12});
}

TEST(SharedMemoryTest, WriteOutOfBoundsReturnsFalse) {
    auto result = shm_type::create(unique_shm_name(), 64);
    ASSERT_TRUE(result.has_value());

    auto& shm = *result;
    const std::array<std::byte, 4> in{};

    EXPECT_FALSE(shm.write(65, in));
    EXPECT_FALSE(shm.write(61, in));
    EXPECT_TRUE(shm.write(60, in));
}

TEST(SharedMemoryTest, ViewInBounds) {
    auto result = shm_type::create(unique_shm_name(), 128);
    ASSERT_TRUE(result.has_value());

    auto& shm = *result;
    auto v = shm.view(10, 20);
    EXPECT_EQ(v.size(), 20u);
    EXPECT_EQ(v.data(), shm.get_memory().data() + 10);
}

TEST(SharedMemoryTest, ViewOutOfBoundsReturnsEmpty) {
    auto result = shm_type::create(unique_shm_name(), 64);
    ASSERT_TRUE(result.has_value());

    auto& shm = *result;
    EXPECT_TRUE(shm.view(64, 1).empty());
    EXPECT_TRUE(shm.view(60, 10).empty());
    EXPECT_TRUE(shm.view(0, 65).empty());
}

TEST(SharedMemoryTest, MoveConstructor) {
    auto result = shm_type::create(unique_shm_name(), 512);
    ASSERT_TRUE(result.has_value());

    shm_type src = std::move(*result);
    shm_type dst(std::move(src));

    EXPECT_EQ(dst.size(), 512u);
    EXPECT_TRUE(src.empty());
}

TEST(SharedMemoryTest, MoveAssignment) {
    auto result = shm_type::create(unique_shm_name(), 256);
    ASSERT_TRUE(result.has_value());

    shm_type src = std::move(*result);
    shm_type dst;
    dst = std::move(src);

    EXPECT_EQ(dst.size(), 256u);
    EXPECT_TRUE(src.empty());
}

TEST(SharedMemoryTest, OpenNonExistentFails) {
    auto result = shm_type::open("/nonexistent_shm_segment_12345");
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().kind(), shared_memory::errc::open_failed);
}

TEST(SharedMemoryTest, CreateDuplicateNameFails) {
    const std::string name = unique_shm_name();
    auto first = shm_type::create(name, 64);
    ASSERT_TRUE(first.has_value());

    auto second = shm_type::create(name, 64);
    EXPECT_FALSE(second.has_value());
    EXPECT_EQ(second.error().kind(), shared_memory::errc::open_failed);
}

} // namespace
