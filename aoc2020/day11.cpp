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

    [[nodiscard]] bool is_inside(int row, int col) const {
        return 0 <= row && row < height() && 0 <= col && col < width();
    }

    [[nodiscard]] seat get(int row, int col) const {
        if (is_inside(row, col)) {
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

bool evolve_close(seating_area const& seats, int row, int col) {
    if (seats.get(row, col) == seat::empty && occupied_neighbours(seats, row, col) == 0) {
        return true;
    } else if (seats.get(row, col) == seat::occupied && occupied_neighbours(seats, row, col) >= 4) {
        return true;
    } else {
        return false;
    }
}

using evolution_function = std::function<bool(seating_area const& seats, int row, int col)>;

seat evolved(seat const& s) {
    if (s == seat::empty) {
        return seat::occupied;
    } else if (s == seat::occupied) {
        return seat::empty;
    } else {
        return s;
    }
}

seating_area evolve(seating_area const& seats, evolution_function const& evolve_fn) {
    seating_area ret{{}, seats.width()};
    for (int row = 0; row < seats.height(); ++row) {
        for (int col = 0; col < seats.width(); ++col) {
            if (evolve_fn(seats, row, col)) {
                ret.seats.push_back(evolved(seats.get(row, col)));
            } else {
                ret.seats.push_back(seats.get(row, col));
            }
        }
    }
    return ret;
}

seating_area evolve_until_stable(seating_area area, evolution_function const& evolution_fn) {
    while (true) {
        seating_area next = evolve(area, evolution_fn);
        if (area.seats == next.seats) {
            return next;
        }
        area = std::move(next);
    }
}

void run() {
    seating_area const area = parse_seats(std::cin);

    auto const stable1 = evolve_until_stable(area, evolve_close);
    std::cout << std::count(stable1.seats.begin(), stable1.seats.end(), seat::occupied) << std::endl;
}
