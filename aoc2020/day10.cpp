#include "input_helpers.hpp"
#include <vector>

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

void run() {
    std::vector<int> const adapters = read_adapters(std::cin);
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
        }
        input_jolts = adapter;
    }

    std::cout << diff1 * (diff3 + 1) << std::endl;
}