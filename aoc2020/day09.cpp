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

int find_invalid(std::vector<int> const& numbers, int preamble_length) {
    xmas xmas;
    size_t i{};
    for (i = 0; i < preamble_length && i < numbers.size(); ++i) {
        xmas.push_back(numbers[i]);
    }
    for (; i < numbers.size(); ++i) {
        if (!push(xmas, numbers[i])) {
            return numbers[i];
        }
    }
    return -1;
}

std::pair<size_t, size_t> find_pairs_summing_to(std::vector<long> const& partial_sums, int target) {
    for (size_t i = 0; i < partial_sums.size(); ++i) {
        for (size_t j = i+2; j < partial_sums.size(); ++j) {
            if (partial_sums[j] - partial_sums[i] == target) {
                return {i, j};
            }
        }
    }
    return {-1, -1};
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
    size_t const preamble_length = 25;
    int const invalid = find_invalid(numbers, preamble_length);
    std::cout << invalid << std::endl;

    std::vector<long> partial_sums{0};
    for (int number : numbers) {
        partial_sums.push_back(partial_sums.back() + number);
    }

    auto [i, j] = find_pairs_summing_to(partial_sums, invalid);

    if (i < partial_sums.size() && j < partial_sums.size()) {
        auto smallest = std::min_element(numbers.begin() + i, numbers.begin() + j);
        auto largest = std::max_element(numbers.begin() + i, numbers.begin() + j);
        std::cout << (*smallest + *largest) << std::endl;
    }
}
