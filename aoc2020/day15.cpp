#include "input_helpers.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <iterator>
#include <algorithm>

std::vector<int> parse(std::istream& is) {
    return std::vector<int>(std::istream_iterator<int>{is}, std::istream_iterator<int>{});
}

void run() {
    auto const starting_numbers = parse(std::cin);

    std::unordered_map<int, size_t> last_mention_of;

    int last_number = starting_numbers.empty() ? 0 : starting_numbers.front();

    for (size_t iteration = 1; iteration < 2020; ++iteration) {
        auto it = last_mention_of.find(last_number);
        if (iteration < starting_numbers.size()) {
            last_mention_of[last_number] = iteration-1;
            last_number = starting_numbers[iteration];
        } else if (it == last_mention_of.end()) {
            last_mention_of[last_number] = iteration-1;
            last_number = 0;
        } else {
            size_t const last_use = it->second;
            last_mention_of[last_number] = iteration - 1;
            last_number = static_cast<int>(iteration - 1 - last_use);
        }
    }
    std::cout << last_number << std::endl;
}
