#include <iostream>
#include <iterator>
#include <vector>

std::pair<long, long> find_pair_summing_to(std::vector<long> const& input, long target) {
    for (size_t i = 0; i < input.size(); ++i) {
        for (size_t j = i+1; j < input.size(); ++j) {
            if (input[i] + input[j] == target) {
                return {input[i], input[j]};
            }
        }
    }
    return {-1, -1};
}

int main() {
    std::vector<long> input;
    std::copy(std::istream_iterator<long>(std::cin), std::istream_iterator<long>(), std::back_inserter(input));

    if (input.empty()) {
        std::cout << "No input\n";
        return 1;
    }

    auto part1 = find_pair_summing_to(input, 2020);
    std::cout << part1.first * part1.second << "\n";
}
