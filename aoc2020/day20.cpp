#include "grid.hpp"
#include "range_helpers.hpp"
#include <iostream>
#include <vector>
#include <string>

struct puzzle_piece {
    int id = 0;
    grid<char> contents;

    bool top_border = false;
    bool right_border = false;
    bool bottom_border = false;
    bool left_border = false;

    [[nodiscard]] std::vector<char> top() const {
        return row(contents, 0);
    }

    [[nodiscard]] std::vector<char> right() const {
        return col(contents, contents.cols() - 1);
    }

    [[nodiscard]] std::vector<char> bottom() const {
        return row(contents, contents.rows() - 1);
    }

    [[nodiscard]] std::vector<char> left() const {
        return col(contents, 0);
    }


};

using piece_collection = std::vector<puzzle_piece>;

piece_collection parse(std::istream& is) {
    piece_collection pieces;
    while (is) {
        std::string line;
        if (std::getline(is, line)) {
            int const id = atoi(line.substr(std::string("Tile ").length()).c_str());
            grid_builder<char> builder;

            while (std::getline(is, line) && !line.empty()) {
                for (char ch : line) {
                    builder.push_back(ch);
                }
                builder.finish_row();
            }
            pieces.push_back({id, builder.build()});
        }
    }
    return pieces;
}

bool matches(std::vector<char> const& edge1, std::vector<char> const& edge2) {
    if (edge1 == edge2) {
        return true;
    } else {
        std::vector<char> reversed = edge1;
        std::reverse(reversed.begin(), reversed.end());
        return reversed == edge2;
    }
}

bool matches(puzzle_piece const& piece, std::vector<char> const& edge) {
    return
        matches(piece.top(), edge) ||
        matches(piece.right(), edge) ||
        matches(piece.bottom(), edge) ||
        matches(piece.left(), edge);
}

void paint_borders(piece_collection& pieces) {
    for (puzzle_piece& piece : pieces) {
        int const id = piece.id;
        auto const top = piece.top();
        auto const right = piece.right();
        auto const bottom = piece.bottom();
        auto const left = piece.left();
        piece.top_border = pieces.end() == std::find_if(pieces.begin(), pieces.end(), [&](puzzle_piece const& other_piece) {
            return id != other_piece.id && matches(other_piece, top);
        });
        piece.right_border = pieces.end() == std::find_if(pieces.begin(), pieces.end(), [&](puzzle_piece const& other_piece) {
            return id != other_piece.id && matches(other_piece, right);
        });
        piece.bottom_border = pieces.end() == std::find_if(pieces.begin(), pieces.end(), [&](puzzle_piece const& other_piece) {
            return id != other_piece.id && matches(other_piece, bottom);
        });
        piece.left_border = pieces.end() == std::find_if(pieces.begin(), pieces.end(), [&](puzzle_piece const& other_piece) {
            return id != other_piece.id && matches(other_piece, left);
        });
    }
}

bool is_corner_piece(puzzle_piece const& piece) {
    return piece.top_border + piece.right_border + piece.bottom_border + piece.left_border == 2;
}

void run() {
    auto pieces = parse(std::cin);
    paint_borders(pieces);

    std::cout << (pieces
                  | std::views::filter(is_corner_piece)
                  | std::views::transform([](puzzle_piece const& p) {
                      return p.id;
                  })
                  | accumulate(1L, std::multiplies{}));

}
