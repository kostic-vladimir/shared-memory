#include <gtest/gtest.h>

#include "shared_memory/error.hpp"

#include <cerrno>
#include <system_error>

namespace {

TEST(ErrorTest, ConstructionAndAccessors) {
    const auto code = std::error_code(EPERM, std::generic_category());
    const shared_memory::error err(shared_memory::errc::open_failed, code);

    EXPECT_EQ(err.kind(), shared_memory::errc::open_failed);
    EXPECT_EQ(err.code(), code);
}

TEST(ErrorTest, MessageContainsKindAndCode) {
    const auto code = std::error_code(EACCES, std::generic_category());
    const shared_memory::error err(shared_memory::errc::open_failed, code);

    const std::string msg = err.message();

    EXPECT_TRUE(msg.find("shared memory open failed") != std::string::npos);
    EXPECT_TRUE(msg.find(code.message()) != std::string::npos);
}

TEST(ErrorTest, Equality) {
    const auto code1 = std::error_code(ENOENT, std::generic_category());
    const auto code2 = std::error_code(EACCES, std::generic_category());

    const shared_memory::error err1(shared_memory::errc::open_failed, code1);
    const shared_memory::error err2(shared_memory::errc::open_failed, code1);
    const shared_memory::error err3(shared_memory::errc::map_failed, code1);
    const shared_memory::error err4(shared_memory::errc::open_failed, code2);

    EXPECT_EQ(err1, err2);
    EXPECT_NE(err1, err3);
    EXPECT_NE(err1, err4);
}

TEST(ErrorTest, AllErrcValuesHaveDescriptions) {
    const auto code = std::error_code(0, std::generic_category());

    EXPECT_FALSE(shared_memory::error(shared_memory::errc::open_failed, code).message().empty());
    EXPECT_FALSE(shared_memory::error(shared_memory::errc::truncate_failed, code).message().empty());
    EXPECT_FALSE(shared_memory::error(shared_memory::errc::map_failed, code).message().empty());
    EXPECT_FALSE(shared_memory::error(shared_memory::errc::stat_failed, code).message().empty());
}

} // namespace
