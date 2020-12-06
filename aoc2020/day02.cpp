#include "day02.hpp"
#include <iostream>
#include <ranges>

bool check_policy1(policy const& p, std::string const& password) {
    size_t const count = std::ranges::distance(password | std::views::filter([&p](int ch) { return ch == p.ch; }));
    return p.min <= count && count <= p.max;
}

bool check_policy2(policy const& p, std::string const& password) {
    size_t const ix0 = p.min - 1;
    size_t const ix1 = p.max - 1;
    return (password.at(ix0) == p.ch) ^ (password.at(ix1) == p.ch);
}

void run() {
    std::string line;
    unsigned successful1 = 0;
    unsigned successful2 = 0;
    while (std::getline(std::cin, line)) {
        auto [policy, password] = parse_policy(line);
        if (policy.ch && check_policy1(policy, password)) {
            ++successful1;
        }
        if (policy.ch && check_policy2(policy, password)) {
            ++successful2;
        }
    }
    std::cout << successful1 << std::endl;
    std::cout << successful2 << std::endl;
}
