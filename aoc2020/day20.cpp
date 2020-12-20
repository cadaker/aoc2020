#include "range_helpers.hpp"
#include <iostream>
#include <array>
#include <unordered_map>

constexpr size_t SIDE_LENGTH = 10;

using side_t = unsigned;

struct puzzle_piece {
    std::array<side_t, 4> sides{};
};

side_t flip(side_t edge) {
    side_t flipped = 0;
    for (size_t i = 0; i < SIDE_LENGTH; ++i) {
        flipped <<= 1;
        flipped |= (edge & 1);
        edge >>= 1;
    }
    return flipped;
}

struct side_builder {
    side_t side = 0;
    size_t index = 0;

    void add(char ch) {
        side |= (ch == '#') << index++;
    }

    template<std::forward_iterator Iter>
    void add(Iter begin, Iter end) {
        for (; begin != end; ++begin) {
            add(*begin);
        }
    }
};

std::unordered_map<int, puzzle_piece> parse(std::istream& is) {
    std::unordered_map<int, puzzle_piece> pieces;
    while (is) {
        std::string line;
        if (std::getline(is, line)) {
            int const id = atoi(line.substr(std::string("Tile :").length() - 1).c_str());

            bool first = true;
            side_builder top;
            side_builder bottom;
            side_builder left;
            side_builder right;
            while (std::getline(is, line) && !line.empty()) {
                if (first) {
                    first = false;
                    top.add(line.begin(), line.end());
                }
                bottom = {};
                bottom.add(line.begin(), line.end());
                left.add(line.front());
                right.add(line.back());
            }
            pieces[id] = puzzle_piece({top.side, right.side, flip(bottom.side), flip(left.side)});
        }
    }
    return pieces;
}

bool matches(side_t s, puzzle_piece const& p) {
    side_t const f = flip(s);
    return std::find(p.sides.begin(), p.sides.end(), s) != p.sides.end() ||
           std::find(p.sides.begin(), p.sides.end(), f) != p.sides.end();
}

size_t count_matches(int id, side_t s, std::unordered_map<int, puzzle_piece> const& pieces) {
    return std::count_if(pieces.begin(), pieces.end(), [id, s](auto const& p) {
        return p.first != id && matches(s, p.second);
    });
}

bool must_be_corner_piece(int id, std::unordered_map<int, puzzle_piece> const& pieces) {
    puzzle_piece const& p = pieces.at(id);
    return count_matches(id, p.sides[0], pieces) +
           count_matches(id, p.sides[1], pieces) +
           count_matches(id, p.sides[2], pieces) +
           count_matches(id, p.sides[3], pieces)
           == 2;
}

void run() {
    auto const pieces = parse(std::cin);

    auto const corner_pieces = pieces
                         | std::views::keys
                         | std::views::filter([&](int id) { return must_be_corner_piece(id, pieces); })
                         | to_vector;
    if (corner_pieces.size() == 4) {
        std::cout << (corner_pieces | accumulate(1L, std::multiplies<>{})) << std::endl;
    } else {
        std::cout << "Found " << corner_pieces.size() << " definite corner pieces." << std::endl;
    }
}
