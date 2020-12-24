#include "range_helpers.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <ranges>

struct coord_t {
    long x = 0;
    long y = 0;

    inline std::strong_ordering operator<=>(coord_t const& coord) const = default;
};

struct coord_hash {
    [[nodiscard]] size_t operator()(coord_t const& xy) const {
        std::hash<long> h;
        return (h(xy.x) + 1) ^ h(xy.y);
    }
};

std::vector<coord_t> compass_to_coords(std::string const& line) {
    std::vector<coord_t> ret;
    size_t i = 0;
    while (i < line.length()) {
        if (line[i] == 'e') {
            ret.push_back({1, 0});
            ++i;
        } else if (line[i] == 'w') {
            ret.push_back({-1, 0});
            ++i;
        } else if (line[i] == 's' && i + 1 < line.length() && line[i+1] == 'e') {
            ret.push_back({1, -1});
            i += 2;
        } else if (line[i] == 's' && i + 1 < line.length() && line[i+1] == 'w') {
            ret.push_back({0, -1});
            i += 2;
        } else if (line[i] == 'n' && i + 1 < line.length() && line[i+1] == 'e') {
            ret.push_back({0, 1});
            i += 2;
        } else if (line[i] == 'n' && i + 1 < line.length() && line[i+1] == 'w') {
            ret.push_back({-1, 1});
            i += 2;
        } else {
            std::cout << "compass_to_coords: parse error\n";
            ++i;
        }
    }
    return ret;
}

enum colour {
    white,
    black,
};

using map_t = std::unordered_map<coord_t, colour, coord_hash>;

void flip(map_t& map, coord_t coord) {
    colour& current = map[coord];
    current = current == white ? black : white;
}

void flip_path(map_t& map, std::vector<coord_t> const& path) {
    coord_t coord;
    for (coord_t diff : path) {
        coord.x += diff.x;
        coord.y += diff.y;
    }
    flip(map, coord);
}

void run() {
    map_t map;
    std::string line;
    while (std::getline(std::cin, line)) {
        flip_path(map, compass_to_coords(line));
    }
    size_t count = 0;
    for (auto& [coord, c] : map) {
        count += (c == black);
    }
    std::cout << count << std::endl;
}
