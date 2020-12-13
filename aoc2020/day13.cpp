#include "range_helpers.hpp"
#include <vector>
#include <utility>
#include <iostream>
#include <regex>
#include <iterator>
#include <algorithm>
#include <ranges>

constexpr long x = -1;

struct input {
    long start_time{};
    std::vector<long> buses;
};

input parse_input(std::istream& is) {
    std::string start_time;
    std::string bus_table;
    if (std::getline(is, start_time) && std::getline(is, bus_table)) {
        static std::regex const bus_pattern(R"(([0-9]+|x))");
        std::vector<long> buses;
        std::transform(
                std::sregex_iterator(bus_table.begin(), bus_table.end(), bus_pattern),
                std::sregex_iterator{},
                std::back_inserter(buses),
                [](std::smatch const& m) {
                    std::string const& s = m.str();
                    return s == "x" ? x : atol(s.c_str());
                });
        return {std::atol(start_time.c_str()), std::move(buses)};
    } else {
        return {};
    }
}

long earliest_bus_time(long start_time, long bus) {
    if (bus != x) {
        long const missed_by = start_time % bus;
        return start_time + (missed_by == 0 ? 0 : bus - missed_by);
    } else {
        return std::numeric_limits<long>::max();
    }
}

long earliest_departure_bus(long start_time, std::vector<long> const& buses) {
    auto it = std::min_element(buses.begin(), buses.end(), [start_time](long bus0, long bus1) {
        return earliest_bus_time(start_time, bus0) < earliest_bus_time(start_time, bus1);
    });
    if (it != buses.end()) {
        return *it;
    } else {
        return x;
    }
}

void run() {
    input const input = parse_input(std::cin);

    long const earliest_bus = earliest_departure_bus(input.start_time, input.buses);
    std::cout << (earliest_bus * (earliest_bus_time(input.start_time, earliest_bus) - input.start_time)) << std::endl;
}
