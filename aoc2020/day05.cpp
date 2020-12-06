#include "input_views.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <unordered_set>

struct seat {
    int row;
    int column;
};

seat parse_seat(std::string const& line) {
    if (line.size() != 10) {
        return {-1, -1};
    }
    int row_begin = 0;
    int row_end = 128;

    for (size_t i = 0; i < 7; ++i) {
        int const mid = (row_begin + row_end) / 2;
        if (line[i] == 'F') {
            row_end = mid;
        } else {
            row_begin = mid;
        }
    }

    int col_begin = 0;
    int col_end = 8;
    for (size_t i = 7; i < 10; ++i)  {
        int const mid = (col_begin + col_end) / 2;
        if (line[i] == 'L') {
            col_end = mid;
        } else {
            col_begin = mid;
        }
    }
    return {row_begin, col_begin};
}

void run() {

    std::vector<seat> seats;
    for (auto seat : input_lines(std::cin) | std::views::transform(parse_seat)) {
        seats.push_back(std::move(seat));
    }
    if (seats.empty()) {
        return;
    }

    auto seat_ids = seats | std::views::transform([](seat const& s) { return s.row * 8 + s.column; });
    std::cout << *std::max_element(seat_ids.begin(), seat_ids.end()) << std::endl;

    int const max_id = *std::max_element(seat_ids.begin(), seat_ids.end());
    int const min_id = *std::min_element(seat_ids.begin(), seat_ids.end());
    std::unordered_set const seat_ids_set(seat_ids.begin(), seat_ids.end());

    for (int id = min_id; id <= max_id; ++id) {
        if (seat_ids_set.count(id) == 0) {
            std::cout << id << std::endl;
            break;
        }
    }
}
