#include "input_helpers.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

enum class seat {
    empty,
    occupied,
    floor,
};

struct seating_area {
    std::vector<seat> seats;
    int _width{};

    [[nodiscard]] int width() const { return _width; }
    [[nodiscard]] int height() const { return static_cast<int>(seats.size()) / _width; }

    [[nodiscard]] seat get(int row, int col) const {
        if (0 <= row && row < height() && 0 <= col && col < width()) {
            return seats[row*_width + col];
        } else {
            return seat::floor;
        }
    }
};

std::pair<int, int> index_to_row_col(int index, int width) {
    return {index % width, index / width};
}

seating_area parse_seats(std::istream& is) {
    seating_area ret;
    for (std::string const& line : input_lines(is)) {
        ret._width = line.length();
        std::transform(line.begin(), line.end(), std::back_inserter(ret.seats), [](int ch) {
            if (ch == 'L') {
                return seat::empty;
            } else if (ch == '#') {
                return seat::occupied;
            } else {
                return seat::floor;
            }
        });
    }
    return ret;
}

int occupied_neighbours(seating_area const& seats, int row, int col) {
    return (seats.get(row-1, col-1) == seat::occupied) +
           (seats.get(row-1, col  ) == seat::occupied) +
           (seats.get(row-1, col+1) == seat::occupied) +
           (seats.get(row  , col-1) == seat::occupied) +
           (seats.get(row  , col+1) == seat::occupied) +
           (seats.get(row+1, col-1) == seat::occupied) +
           (seats.get(row+1, col  ) == seat::occupied) +
           (seats.get(row+1, col+1) == seat::occupied);
}

seating_area evolve(seating_area const& seats) {
    seating_area ret{{}, seats.width()};
    for (int row = 0; row < seats.height(); ++row) {
        for (int col = 0; col < seats.width(); ++col) {
            if (seats.get(row, col) == seat::empty && occupied_neighbours(seats, row, col) == 0) {
                ret.seats.push_back(seat::occupied);
            } else if (seats.get(row, col) == seat::occupied && occupied_neighbours(seats, row, col) >= 4) {
                ret.seats.push_back(seat::empty);
            } else {
                ret.seats.push_back(seats.get(row, col));
            }
        }
    }
    return ret;
}

void run() {
    seating_area area = parse_seats(std::cin);

    while (true) {
        seating_area next = evolve(area);
        if (area.seats == next.seats) {
            std::cout << std::count(area.seats.begin(), area.seats.end(), seat::occupied) << std::endl;
            break;
        }
        area = std::move(next);
    }
}
