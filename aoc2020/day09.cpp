#include "input_helpers.hpp"
#include <iostream>
#include <deque>
#include <vector>
#include <iterator>

using xmas = std::deque<int>;

[[nodiscard]] bool has_match(xmas const& xmas, int number) {
    for (auto it = xmas.begin(); it != xmas.end(); ++it) {
        for(auto it2 = std::next(it); it2 != xmas.end(); ++it2) {
            if ((*it) + (*it2) == number) {
                return true;
            }
        }
    }
    return false;
}

bool push(xmas& xmas, int number) {
    bool const ret = has_match(xmas, number);
    xmas.push_back(number);
    xmas.pop_front();
    return ret;
}

void run() {
    std::vector<int> numbers;
    std::transform(
            input_line_iterator{std::cin},
            input_line_iterator{},
            std::back_inserter(numbers),
            [](std::string const& line) {
                return atoi(line.c_str());
            });
    size_t const N = 25;
    xmas xmas;
    size_t i{};
    for (i = 0; i < N && i < numbers.size(); ++i) {
        xmas.push_back(numbers[i]);
    }
    for (; i < numbers.size(); ++i) {
        if (!push(xmas, numbers[i])) {
            std::cout << numbers[i] << std::endl;
            break;
        }
    }
}
