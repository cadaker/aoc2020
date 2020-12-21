#include "gtest/gtest.h"
#include "grid.hpp"
#include "range_helpers.hpp"
#include <sstream>

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

TEST(grid, grid_builder) {
    grid_builder<int> builder;
    builder.push_back(0);
    builder.push_back(1);
    builder.push_back(2);
    builder.finish_row();
    builder.push_back(3);
    builder.push_back(4);
    builder.push_back(5);
    builder.finish_row();
    auto grid = builder.build();
    ASSERT_EQ(grid.rows(), 2);
    ASSERT_EQ(grid.cols(), 3);
    EXPECT_EQ(grid(0, 0), 0);
    EXPECT_EQ(grid(0, 1), 1);
    EXPECT_EQ(grid(0, 2), 2);
    EXPECT_EQ(grid(1, 0), 3);
    EXPECT_EQ(grid(1, 1), 4);
    EXPECT_EQ(grid(1, 2), 5);
}

TEST(grid, grid_formatting) {
    grid<int> g(2, 3);
    std::iota(g.begin(), g.end(), 0);
    std::ostringstream oss;
    oss << g;
    EXPECT_EQ(oss.str(), "0 1 2\n3 4 5\n");
}

TEST(grid, rotate) {
    grid<int> g(2, 3);
    std::iota(g.begin(), g.end(), 0);
    grid<int> grot = rotate_ccw(g);
    ASSERT_EQ(grot.cols(), 2);
    ASSERT_EQ(grot.rows(), 3);
    EXPECT_EQ(grot(0, 0), 2);
    EXPECT_EQ(grot(0, 1), 5);
    EXPECT_EQ(grot(1, 0), 1);
    EXPECT_EQ(grot(1, 1), 4);
    EXPECT_EQ(grot(2, 0), 0);
    EXPECT_EQ(grot(2, 1), 3);
}

TEST(grid, mirror_horiz) {
    grid<int> g(2, 3);
    std::iota(g.begin(), g.end(), 0);
    grid<int> gflip = mirror_horiz(g);
    ASSERT_EQ(gflip.cols(), 3);
    ASSERT_EQ(gflip.rows(), 2);
    EXPECT_EQ(gflip(0, 0), 2);
    EXPECT_EQ(gflip(0, 1), 1);
    EXPECT_EQ(gflip(0, 2), 0);
    EXPECT_EQ(gflip(1, 0), 5);
    EXPECT_EQ(gflip(1, 1), 4);
    EXPECT_EQ(gflip(1, 2), 3);
}

TEST(grid, mirror_vert) {
    grid<int> g(2, 3);
    std::iota(g.begin(), g.end(), 0);
    grid<int> gflip = mirror_vert(g);
    ASSERT_EQ(gflip.cols(), 3);
    ASSERT_EQ(gflip.rows(), 2);
    EXPECT_EQ(gflip(0, 0), 3);
    EXPECT_EQ(gflip(0, 1), 4);
    EXPECT_EQ(gflip(0, 2), 5);
    EXPECT_EQ(gflip(1, 0), 0);
    EXPECT_EQ(gflip(1, 1), 1);
    EXPECT_EQ(gflip(1, 2), 2);
}
