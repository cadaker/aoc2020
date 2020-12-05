#include <iostream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>

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

int main() {

    std::vector<seat> seats;
    std::string line;
    while (std::getline(std::cin, line)) {
        seats.push_back(parse_seat(line));
    }
    if (seats.empty()) {
        return 1;
    }

    auto seat_ids = seats | std::views::transform([](seat const& s) { return s.row * 8 + s.column; });
    std::cout << *std::max_element(seat_ids.begin(), seat_ids.end()) << std::endl;
}
