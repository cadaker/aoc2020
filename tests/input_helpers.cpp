#include "gtest/gtest.h"
#include "input_helpers.hpp"

#include <sstream>
#include <vector>

TEST(input_views, input_lines_range) {
    std::istringstream iss(R"(foo
bar
baz
qux quux quuux)");
    input_lines line_range(iss);
    std::vector lines(line_range.begin(), line_range.end());

    ASSERT_EQ(lines.size(), 4);
    EXPECT_EQ(lines[0], "foo");
    EXPECT_EQ(lines[1], "bar");
    EXPECT_EQ(lines[2], "baz");
    EXPECT_EQ(lines[3], "qux quux quuux");
}
