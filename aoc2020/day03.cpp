#include <iostream>
#include <string>
#include <vector>


struct toboggan_map {
    std::vector<std::string> rows;

    [[nodiscard]] bool has_tree(unsigned row, unsigned column) const {
        if (row < rows.size() && !rows[row].empty()) {
            return rows[row][column % rows[row].size()] == '#';
        } else {
            return false;
        }
    }
};

toboggan_map parse_map(std::istream& is) {
    std::string line;
    toboggan_map map;
    while (std::getline(is, line)) {
        map.rows.push_back(std::move(line));
    }
    return map;
}

unsigned count_trees(toboggan_map const& tmap, unsigned row_step, unsigned column_step) {
    unsigned tree_count = 0;
    for (unsigned row = 0, column = 0; row < tmap.rows.size(); row += row_step, column += column_step) {
        if (tmap.has_tree(row, column)) {
            ++tree_count;
        }
    }
    return tree_count;
}

int main() {
    toboggan_map const tmap = parse_map(std::cin);
    if (tmap.rows.empty()) {
        std::cerr << "Bad map" << std::endl;
        return 1;
    }

    std::cout << count_trees(tmap, 1, 3) << std::endl;
}
