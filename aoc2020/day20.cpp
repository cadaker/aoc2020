#include "grid.hpp"
#include "range_helpers.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

struct puzzle_piece {
    int id = 0;
    grid<char> contents;

    bool top_border = false;
    bool right_border = false;
    bool bottom_border = false;
    bool left_border = false;

    explicit operator bool() const {
        return static_cast<bool>(contents);
    }

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

puzzle_piece rotate_ccw(puzzle_piece const& p) {
    return {
            p.id,
            rotate_ccw(p.contents),
            p.right_border,
            p.bottom_border,
            p.left_border,
            p.top_border,
    };
}

puzzle_piece mirror_horiz(puzzle_piece const& p) {
    return {
            p.id,
            mirror_horiz(p.contents),
            p.top_border,
            p.left_border,
            p.bottom_border,
            p.right_border,
    };
}

puzzle_piece mirror_vert(puzzle_piece const& p) {
    return {
            p.id,
            mirror_vert(p.contents),
            p.bottom_border,
            p.right_border,
            p.top_border,
            p.left_border,
    };
}

std::vector<puzzle_piece> variations(puzzle_piece piece) {
    std::vector<puzzle_piece> ret;
    for (int i = 0; i < 4; ++i) {
        ret.push_back(piece);
        piece = rotate_ccw(piece);
    }
    piece = mirror_horiz(piece);
    for (int i = 0; i < 4; ++i) {
        ret.push_back(piece);
        piece = rotate_ccw(piece);
    }
    piece = mirror_vert(piece);
    for (int i = 0; i < 4; ++i) {
        ret.push_back(piece);
        piece = rotate_ccw(piece);
    }
    piece = mirror_horiz(piece);
    for (int i = 0; i < 4; ++i) {
        ret.push_back(piece);
        piece = rotate_ccw(piece);
    }
    return ret;
}

puzzle_piece fit_piece(
        piece_collection const& pieces,
        std::vector<char> const& left,
        std::vector<char> const& top,
        std::unordered_set<int> skip_ids) {
    for (auto const& piece : pieces) {
        for (puzzle_piece const& piece_variation : variations(piece)) {
            if (skip_ids.count(piece_variation.id) == 0 &&
                (left.empty() && piece_variation.left_border || left == piece_variation.left()) &&
                (top.empty() && piece_variation.top_border || top == piece_variation.top())) {
                return piece_variation;
            }
        }
    }
    return {};
}

grid<puzzle_piece> solve(piece_collection const& pieces) {
    std::vector<puzzle_piece> placed_pieces;
    size_t width = 0;
    std::unordered_set<int> used_ids;
    while (placed_pieces.size() < pieces.size()) {
        auto const left = placed_pieces.empty() || placed_pieces.back().right_border ? std::vector<char>{} : placed_pieces.back().right();
        auto const top = width == 0 ? std::vector<char>{} : placed_pieces.at(placed_pieces.size() - width).bottom();

        if (auto p = fit_piece(pieces, left, top, used_ids)) {
            used_ids.insert(p.id);
            placed_pieces.push_back(std::move(p));
            if (width == 0 && placed_pieces.back().right_border) {
                width = placed_pieces.size();
            }
        } else {
            return {};
        }
    }

    grid<puzzle_piece> ret(placed_pieces.size() / width, width);
    std::copy(placed_pieces.begin(), placed_pieces.end(), ret.begin());
    return ret;
}

grid<char> join_puzzle(grid<puzzle_piece> const& puzzle) {
    std::vector<grid<char>> joined_rows;
    auto crop = [](grid<char> const& g) {
        return subgrid(g, 1, g.rows() - 1, 1, g.cols() - 1);
    };
    for (size_t r = 0; r < puzzle.rows(); ++r) {
        auto g = crop(puzzle(r, 0).contents);
        for (size_t c = 1; c < puzzle.cols(); ++c) {
            g = join_horiz(g, crop(puzzle(r, c).contents));
        }
        joined_rows.push_back(std::move(g));
    }

    grid<char> ret = joined_rows.at(0);
    for (size_t i = 1; i < joined_rows.size(); ++i) {
        ret = join_vert(ret, joined_rows[i]);
    }
    return ret;
}

static const std::string monster_1("                  # ");
static const std::string monster_2("#    ##    ##    ###");
static const std::string monster_3(" #  #  #  #  #  #   ");

bool match_sea_monster(grid<char> const& map, size_t r, size_t c) {
    if (c + monster_1.length() < map.cols() && r + 2 < map.rows()) {
        for (size_t i = 0; i < monster_1.size(); ++i) {
            if ((monster_1[i] == '#' && map(r, c + i) == '.' ) ||
                (monster_2[i] == '#' && map(r + 1, c + i) == '.') ||
                (monster_3[i] == '#' && map(r + 2, c + i) == '.')) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

void paint_sea_monster(grid<char>& map, size_t r, size_t c) {
    for (size_t i = 0; i < monster_1.size(); ++i) {
        if (monster_1[i] == '#') {
            map(r, c + i) = 'O';
        }
        if (monster_2[i] == '#') {
            map(r + 1, c + i) = 'O';
        }
        if (monster_3[i] == '#') {
            map(r + 2, c + i) = 'O';
        }
    }
}

void find_and_paint_sea_monsters(grid<char>& map) {
    for (size_t r = 0; r < map.rows(); ++r) {
        for (size_t c = 0; c < map.cols(); ++c) {
            if (match_sea_monster(map, r, c)) {
                paint_sea_monster(map, r, c);
            }
        }
    }
}

void find_and_paint_all_sea_monsters(grid<char>& map) {
    for (int i = 0; i < 4; ++i) {
        find_and_paint_sea_monsters(map);
        map = rotate_ccw(map);
    }
    map = mirror_horiz(map);
    for (int i = 0; i < 4; ++i) {
        find_and_paint_sea_monsters(map);
        map = rotate_ccw(map);
    }
    map = mirror_vert(map);
    for (int i = 0; i < 4; ++i) {
        find_and_paint_sea_monsters(map);
        map = rotate_ccw(map);
    }
    map = mirror_horiz(map);
    for (int i = 0; i < 4; ++i) {
        find_and_paint_sea_monsters(map);
        map = rotate_ccw(map);
    }
}

void run() {
    auto pieces = parse(std::cin);
    paint_borders(pieces);

    std::cout << (pieces
                  | std::views::filter(is_corner_piece)
                  | std::views::transform([](puzzle_piece const& p) {
                      return p.id;
                  })
                  | accumulate(1L, std::multiplies{})) << std::endl;

    auto puzzle = solve(pieces);

    grid<char> map = join_puzzle(puzzle);

    find_and_paint_all_sea_monsters(map);

    std::cout << std::count(map.begin(), map.end(), '#') << std::endl;
}
