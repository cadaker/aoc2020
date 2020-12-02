#include "gtest/gtest.h"
#include "day02.hpp"

TEST(day02, parse_policy) {
    {
        auto [policy, password] = parse_policy("1-2 a: aaabc");
        EXPECT_EQ(policy.ch, 'a');
        EXPECT_EQ(policy.min, 1);
        EXPECT_EQ(policy.max, 2);
        EXPECT_EQ(password, "aaabc");
    }
}
