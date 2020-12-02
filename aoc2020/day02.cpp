#include "day02.hpp"
#include <iostream>
#include <ranges>

bool check_policy(policy const& p, std::string const& password) {
    size_t count = std::ranges::distance(password | std::views::filter([&p](int ch) { return ch == p.ch; }));
    return p.min <= count && count <= p.max;
}

int main() {
    std::string line;
    unsigned successful = 0;
    while (std::getline(std::cin, line)) {
        auto [policy, password] = parse_policy(line);
        if (policy.ch && check_policy(policy, password)) {
            ++successful;
        }
    }
    std::cout << successful << std::endl;
}
