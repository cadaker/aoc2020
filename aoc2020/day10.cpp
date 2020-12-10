#include "input_helpers.hpp"
#include "range_helpers.hpp"
#include <vector>
#include <tuple>
#include <algorithm>
#include <deque>
#include <numeric>

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

size_t count_chains_between(std::vector<int> const& adaptors, int start, int target) {
    auto adaptors_begin = std::lower_bound(adaptors.begin(), adaptors.end(), start + 1);
    auto adaptors_end = std::lower_bound(adaptors.begin(), adaptors.end(), target);

    size_t count = 0;
    std::deque<int> queue{start};
    while (!queue.empty()) {
        int const jolts = queue.front();
        queue.pop_front();
        if (jolts + 3 >= target) {
            count++;
        }
        for (int adaptor : pairseq(adaptors_begin, adaptors_end)) {
            if (jolts + 1 <= adaptor && jolts + 3 >= adaptor) {
                queue.push_back(adaptor);
            }
        }
    }
    return count;
}

void run() {
    std::vector<int> const adapters = read_adapters(std::cin);

    auto [diff1, diff2, diff3] = adapter_diffs(adapters);

    std::cout << diff1 * (diff3 + 1) << std::endl;

    auto sorted_adapters = adapters;
    std::sort(sorted_adapters.begin(), sorted_adapters.end());

    int const laptop_jolts = sorted_adapters.empty() ? 0 : (sorted_adapters.back() + 3);

    std::vector<size_t> chain_snippet_counts;

    int low_jolts = 0;
    for (size_t i = 1; i + 1 < sorted_adapters.size(); ++i) {
        if (sorted_adapters[i+1] - sorted_adapters[i-1] > 3) {
            chain_snippet_counts.push_back(count_chains_between(sorted_adapters, low_jolts, sorted_adapters[i]));
            low_jolts = sorted_adapters[i];
        }
    }
    chain_snippet_counts.push_back(count_chains_between(sorted_adapters, low_jolts, laptop_jolts));

    std::cout << std::accumulate(chain_snippet_counts.begin(), chain_snippet_counts.end(), 1UL, std::multiplies<>{}) << std::endl;

}
