#include "gtest/gtest.h"
#include "grid.hpp"
#include "range_helpers.hpp"

TEST(grid, basics) {
    grid<int> grid(3, 2);
    ASSERT_EQ(grid.cols(), 2);
    ASSERT_EQ(grid.rows(), 3);
    for (size_t i = 0; i < grid.size(); ++i) {
        *(grid.begin() + i) = static_cast<int>(i);
    }
    EXPECT_EQ(grid(0, 0), 0);
    EXPECT_EQ(grid(0, 1), 1);
    EXPECT_EQ(grid(1, 0), 2);
    EXPECT_EQ(grid(1, 1), 3);
    EXPECT_EQ(grid(2, 0), 4);
    EXPECT_EQ(grid(2, 1), 5);
}

TEST(grid, row_col_access) {
    grid<int> grid(3, 2);
    std::iota(grid.begin(), grid.end(), 0);

    EXPECT_EQ(row(grid, 2), (std::vector<int>{4, 5}));
    EXPECT_EQ(col(grid, 1), (std::vector<int>{1, 3, 5}));
}

TEST(grid, join) {
    grid<int> grid32(3, 2);
    grid<int> grid33(3, 3);
    grid<int> grid22(2, 2);
    std::iota(grid32.begin(), grid32.end(), 10);
    std::iota(grid33.begin(), grid33.end(), 20);
    std::iota(grid22.begin(), grid22.end(), 30);

    auto grid35 = join_horiz(grid32, grid33);
    ASSERT_EQ(grid35.rows(), 3);
    ASSERT_EQ(grid35.cols(), 5);
    EXPECT_EQ(grid35(0, 0), 10);
    EXPECT_EQ(grid35(2, 4), 28);

    auto grid52 = join_vert(grid32, grid22);
    ASSERT_EQ(grid52.rows(), 5);
    ASSERT_EQ(grid52.cols(), 2);
    EXPECT_EQ(grid52(0, 0), 10);
    EXPECT_EQ(grid52(4, 1), 33);
}

TEST(grid, subgrid) {
    grid<int> grid(4, 4);
    std::iota(grid.begin(), grid.end(), 0);
    auto grid2 = subgrid(grid, 1, 3, 1, 3);
    ASSERT_EQ(grid2.rows(), 2);
    ASSERT_EQ(grid2.cols(), 2);
    EXPECT_EQ(grid2(0, 0), 5);
    EXPECT_EQ(grid2(0, 1), 6);
    EXPECT_EQ(grid2(1, 0), 9);
    EXPECT_EQ(grid2(1, 1), 10);
}
