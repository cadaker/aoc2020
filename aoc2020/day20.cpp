#include "range_helpers.hpp"
#include <iostream>
#include <array>
#include <unordered_map>

constexpr size_t SIDE_LENGTH = 10;

using side_t = unsigned;

constexpr side_t BORDER = ~0;
constexpr side_t ANY = BORDER - 1;


struct puzzle_piece {
    int id = 0;
    std::array<side_t, 4> sides{};
    std::vector<char> contents;

    explicit operator bool() const {
        return !contents.empty();
    }
};

using piece_collection = std::unordered_map<int, puzzle_piece>;

side_t flip(side_t edge) {
    if (edge == BORDER || edge == ANY) {
        return edge;
    } else {
        side_t flipped = 0;
        for (size_t i = 0; i < SIDE_LENGTH; ++i) {
            flipped <<= 1;
            flipped |= (edge & 1);
            edge >>= 1;
        }
        return flipped;
    }
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

piece_collection parse(std::istream& is) {
    piece_collection pieces;
    while (is) {
        std::string line;
        if (std::getline(is, line)) {
            int const id = atoi(line.substr(std::string("Tile :").length() - 1).c_str());

            std::vector<char> contents;
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

                contents.insert(contents.end(), line.begin(), line.end());
            }
            pieces[id] = puzzle_piece{id, {top.side, right.side, flip(bottom.side), flip(left.side)}, std::move(contents)};
        }
    }
    return pieces;
}

bool matches(side_t s, puzzle_piece const& p) {
    side_t const f = flip(s);
    return std::find(p.sides.begin(), p.sides.end(), s) != p.sides.end() ||
           std::find(p.sides.begin(), p.sides.end(), f) != p.sides.end();
}

size_t count_matches(int id, side_t s, piece_collection const& pieces) {
    return std::count_if(pieces.begin(), pieces.end(), [id, s](auto const& p) {
        return p.first != id && matches(s, p.second);
    });
}

bool must_be_corner_piece(int id, piece_collection const& pieces) {
    puzzle_piece const& p = pieces.at(id);
    return count_matches(id, p.sides[0], pieces) +
           count_matches(id, p.sides[1], pieces) +
           count_matches(id, p.sides[2], pieces) +
           count_matches(id, p.sides[3], pieces)
           == 2;
}

struct full_puzzle {
    size_t width = 0;
    std::vector<char> contents;
};

puzzle_piece rotate_piece(puzzle_piece const& p) {
    // Rotate CCW
    std::vector<char> rotated_contents(p.contents.size(), 0);
    for (size_t row = 0; row < SIDE_LENGTH; ++row) {
        for (size_t col = 0; col < SIDE_LENGTH; ++col) {
            rotated_contents.at((SIDE_LENGTH - 1 - col)*SIDE_LENGTH + row) = p.contents.at(row * SIDE_LENGTH + col);
        }
    }

    return puzzle_piece{p.id, {p.sides[1], p.sides[2], p.sides[3], p.sides[0]}, std::move(rotated_contents)};
}

puzzle_piece flip_piece_lr(puzzle_piece const& p) {
    // Mirror left-right
    if (p.contents.size() != SIDE_LENGTH * SIDE_LENGTH) {
        return {};
    }
    std::vector<char> flipped_contents = p.contents;
    for (size_t row = 0; row < SIDE_LENGTH; ++row) {
        std::reverse(flipped_contents.begin() + row*SIDE_LENGTH, flipped_contents.begin() + (row+1)*SIDE_LENGTH);
    }
    return puzzle_piece{p.id, {flip(p.sides[0]), flip(p.sides[3]), flip(p.sides[2]), flip(p.sides[1])}, std::move(flipped_contents)};
}

puzzle_piece flip_piece_ud(puzzle_piece const& p) {
    // Mirror up-down
    if (p.contents.size() != SIDE_LENGTH * SIDE_LENGTH) {
        return {};
    }
    std::vector<char> flipped_contents(p.contents.size());
    for (size_t row = 0; row < SIDE_LENGTH; ++row) {
        std::copy(p.contents.begin() + row*SIDE_LENGTH, p.contents.begin() + (row+1)*SIDE_LENGTH,
                  flipped_contents.begin() + (SIDE_LENGTH - 1 - row)*SIDE_LENGTH);
    }
    return puzzle_piece{p.id, {flip(p.sides[2]), flip(p.sides[1]), flip(p.sides[0]), flip(p.sides[3])}, std::move(flipped_contents)};
}

bool matches(puzzle_piece const& piece, side_t top, side_t right, side_t bottom, side_t left) {
    // Since the right side counts top-to-bottom, and the left side bottom-to-top - we need to flip here to match properly.
    return
            (top == ANY || piece.sides[0] == flip(top)) &&
            (right == ANY || piece.sides[1] == flip(right)) &&
            (bottom == ANY || piece.sides[2] == flip(bottom)) &&
            (left == ANY || piece.sides[3] == flip(left));
}

puzzle_piece fit_to(puzzle_piece piece, side_t top, side_t right, side_t bottom, side_t left) {
    for (size_t i = 0; i < 4; ++i) {
        if (matches(piece, top, right, bottom, left)) {
            return piece;
        }
        piece = rotate_piece(piece);
    }
    piece = flip_piece_ud(piece);
    for (size_t i = 0; i < 4; ++i) {
        if (matches(piece, top, right, bottom, left)) {
            return piece;
        }
        piece = rotate_piece(piece);
    }
    piece = flip_piece_lr(piece);
    for (size_t i = 0; i < 4; ++i) {
        if (matches(piece, top, right, bottom, left)) {
            return piece;
        }
        piece = rotate_piece(piece);
    }
    piece = flip_piece_ud(piece);
    for (size_t i = 0; i < 4; ++i) {
        if (matches(piece, top, right, bottom, left)) {
            return piece;
        }
        piece = rotate_piece(piece);
    }
    return {};
}

puzzle_piece put_piece(std::vector<puzzle_piece> const& placed_pieces, size_t width, piece_collection const& pieces) {
    side_t const left_match = placed_pieces.empty() ? BORDER : placed_pieces.back().sides[1];
    side_t const top_match = width == 0 || placed_pieces.size() < width ? BORDER : placed_pieces.at(placed_pieces.size() - width).sides[2];

    for (auto const& [id, piece] : pieces) {
        if (placed_pieces.empty() || id != placed_pieces.back().id) {
            if (auto p = fit_to(piece, top_match, ANY, ANY, left_match)) {
                return p;
            }
        }
    }
    return {};
}

full_puzzle join_pieces(std::vector<puzzle_piece> const& placed_pieces, size_t width) {
    std::vector<char> contents;
    size_t const height = placed_pieces.size() / width;
    // First row
    for (size_t col = 0; col < width; ++col) {
        puzzle_piece const& piece = placed_pieces.at(col);
        if (col == 0) {
            contents.push_back(piece.contents.at(0));
        }
        std::copy(piece.contents.begin() + 1,
                  piece.contents.begin() + SIDE_LENGTH,
                  std::back_inserter(contents));
    }

    for (size_t row = 0; row < height; ++row) {
        for (size_t inner_row = 1; inner_row < SIDE_LENGTH; ++inner_row) {
            for (size_t col = 0; col < width; ++col) {
                puzzle_piece const& piece = placed_pieces.at(row * width + col);
                if (col == 0) {
                    contents.push_back(piece.contents.at(inner_row * SIDE_LENGTH));
                }
                std::copy(piece.contents.begin() + 1 + inner_row*SIDE_LENGTH,
                          piece.contents.begin() + (inner_row+1)*SIDE_LENGTH,
                          std::back_inserter(contents));
            }
        }
    }
    return full_puzzle{width * (SIDE_LENGTH-1) + 1, std::move(contents)};
}

full_puzzle solve_puzzle(piece_collection const& pieces) {
    std::vector<puzzle_piece> placed_pieces;
    size_t width = 0;
    while (auto p = put_piece(placed_pieces, width, pieces)) {
        placed_pieces.push_back(std::move(p));
        if (width == 0 && placed_pieces.back().sides[1] == BORDER) {
            width = placed_pieces.size();
        }
        if (placed_pieces.size() == pieces.size()) {
            break;
        }
    }

    if (placed_pieces.size() == pieces.size()) {
        return join_pieces(placed_pieces, width);
    } else {
        return {};
    }
}

side_t identify_border(int id, side_t side, piece_collection const& pieces) {
    if (count_matches(id, side, pieces) == 0) {
        return BORDER;
    } else {
        return side;
    }
}

piece_collection paint_borders(piece_collection const& pieces) {
    piece_collection painted;
    for (auto const& [id, piece] : pieces) {
        painted[id] = puzzle_piece{
            id,
            {
                    identify_border(id, piece.sides[0], pieces),
                    identify_border(id, piece.sides[1], pieces),
                    identify_border(id, piece.sides[2], pieces),
                    identify_border(id, piece.sides[3], pieces),
            },
            piece.contents
        };
    }
    return painted;
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

    auto painted_pieces = paint_borders(pieces);

    auto const puzzle = solve_puzzle(painted_pieces);
}
