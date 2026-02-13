#include <gtest/gtest.h>

#include "owned_fd.hpp"

#include <fcntl.h>
#include <unistd.h>

namespace {

TEST(OwnedFdTest, DefaultConstruction) {
    shared_memory::owned_fd fd;
    EXPECT_EQ(fd.get(), shared_memory::owned_fd::INVALID_FD);
    EXPECT_FALSE(fd.is_valid());
}

TEST(OwnedFdTest, InvalidFdConstruction) {
    shared_memory::owned_fd fd(shared_memory::owned_fd::INVALID_FD);
    EXPECT_EQ(fd.get(), shared_memory::owned_fd::INVALID_FD);
    EXPECT_FALSE(fd.is_valid());
}

TEST(OwnedFdTest, ValidFdConstruction) {
    const int raw_fd = open("/dev/null", O_RDONLY);
    ASSERT_GE(raw_fd, 0) << "open(/dev/null) failed";

    shared_memory::owned_fd fd(raw_fd);
    EXPECT_EQ(fd.get(), raw_fd);
    EXPECT_TRUE(fd.is_valid());
    // fd closes on destruction
}

TEST(OwnedFdTest, MoveConstructor) {
    const int raw_fd = open("/dev/null", O_RDONLY);
    ASSERT_GE(raw_fd, 0);

    shared_memory::owned_fd src(raw_fd);
    shared_memory::owned_fd dst(std::move(src));

    EXPECT_EQ(dst.get(), raw_fd);
    EXPECT_TRUE(dst.is_valid());
    EXPECT_EQ(src.get(), shared_memory::owned_fd::INVALID_FD);
    EXPECT_FALSE(src.is_valid());
}

TEST(OwnedFdTest, MoveAssignment) {
    const int raw_fd = open("/dev/null", O_RDONLY);
    ASSERT_GE(raw_fd, 0);

    shared_memory::owned_fd src(raw_fd);
    shared_memory::owned_fd dst;
    dst = std::move(src);

    EXPECT_EQ(dst.get(), raw_fd);
    EXPECT_TRUE(dst.is_valid());
    EXPECT_EQ(src.get(), shared_memory::owned_fd::INVALID_FD);
    EXPECT_FALSE(src.is_valid());
}

TEST(OwnedFdTest, MoveAssignmentSelf) {
    const int raw_fd = open("/dev/null", O_RDONLY);
    ASSERT_GE(raw_fd, 0);

    shared_memory::owned_fd fd(raw_fd);
    fd = std::move(fd);

    EXPECT_EQ(fd.get(), raw_fd);
    EXPECT_TRUE(fd.is_valid());
}

} // namespace
