#include "input_helpers.hpp"
#include "range_helpers.hpp"
#include <vector>
#include <tuple>
#include <algorithm>
#include <deque>
#include <numeric>
#include <unordered_set>

std::vector<int> read_adapters(std::istream& is) {
    std::vector<int> adapters;
    std::transform(
            input_line_iterator{std::cin},
            input_line_iterator{},
            std::back_inserter(adapters),
            [](std::string const& line) {
                return atoi(line.c_str());
            });
    return adapters;
}

std::tuple<int, int, int> adapter_diffs(std::vector<int> const& adapters) {
    auto sorted_adapters = adapters;
    std::sort(sorted_adapters.begin(), sorted_adapters.end());

    int input_jolts = 0;
    int diff1 = 0;
    int diff2 = 0;
    int diff3 = 0;
    for (int adapter : sorted_adapters) {
        if (adapter - input_jolts == 1) {
            diff1++;
        } else if (adapter - input_jolts == 2) {
            diff2++;
        } else if (adapter - input_jolts == 3) {
            diff3++;
        } else {
            std::cout << "Bad adapter! in: " << input_jolts << ", adapter: " << adapter << std::endl;
            return {-1, -1, -1};
        }
        input_jolts = adapter;
    }
    return {diff1, diff2, diff3};
}

size_t count_chains(std::vector<int> const& adapters) {
    int const laptop_jolts = (adapters.empty() ? 0 : *std::max_element(adapters.begin(), adapters.end())) + 3;

    std::unordered_set<int> extended_jolts(adapters.begin(), adapters.end());
    extended_jolts.insert(0);
    extended_jolts.insert(laptop_jolts);

    std::vector<size_t> chains_until(laptop_jolts + 1, 0);
    chains_until[0] = 1;

    for (int jolts = 1; jolts <= laptop_jolts; ++jolts) {
        if (extended_jolts.count(jolts) > 0) {
            chains_until[jolts] =
                    chains_until[jolts - 1] +
                    (jolts >= 2 ? chains_until[jolts - 2] : 0) +
                    (jolts >= 3 ? chains_until[jolts - 3] : 0);
        }
    }
    return chains_until.back();
}

void run() {
    std::vector<int> const adapters = read_adapters(std::cin);

    auto [diff1, diff2, diff3] = adapter_diffs(adapters);

    std::cout << diff1 * (diff3 + 1) << std::endl;

    std::cout << count_chains(adapters) << std::endl;
}
