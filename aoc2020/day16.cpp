#include "range_helpers.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <sstream>

struct range_t {
    int low = 0;
    int high = 0;
};

using constraint_t = std::pair<range_t, range_t>;

using constraints_t = std::unordered_map<std::string, constraint_t>;

using ticket_t = std::vector<int>;

struct problem_t {
    constraints_t constraints;
    ticket_t your_ticket;
    std::vector<ticket_t> nearby_tickets;
};

ticket_t parse_ticket(std::string const& line) {
    std::istringstream is(line);
    ticket_t ret;

    int tmp{};
    while (is >> tmp) {
        ret.push_back(tmp);
        if (is.get() != ',' && is) {
            std::cout << "Unexpected input while parsing ticket: " << line << std::endl;
        }
    }
    return ret;
}

problem_t parse(std::istream& is) {
    static const std::regex constraint_pattern(R"((.*): ([0-9]+)-([0-9]+) or ([0-9]+)-([0-9]+))");
    std::string line;
    constraints_t constraints;
    auto to_int = [](std::string const& s) {
        return std::atoi(s.c_str());
    };
    while (std::getline(is, line) && !line.empty()) {
        std::smatch m;
        if (std::regex_match(line, m, constraint_pattern)) {
            constraints[m[1].str()] = {
                    {to_int(m[2].str()), to_int(m[3].str())},
                    {to_int(m[4].str()), to_int(m[5].str())}
            };
        } else {
            std::cout << "Failed to parse: " << line << std::endl;
        }
    }

    ticket_t your_ticket;
    line = {};
    if (std::getline(is, line) && line == "your ticket:") {
        std::getline(is, line);
        your_ticket = parse_ticket(line);
    } else {
        std::cout << "Expected your ticket, got " << line << std::endl;
    }

    if (!std::getline(is, line) || !line.empty()) {
        std::cout << "Expected empty line, got " << line << std::endl;
    }

    std::vector<ticket_t> nearby_tickets;
    line = {};
    if (std::getline(is, line) && line == "nearby tickets:") {
        while (std::getline(is, line)) {
            nearby_tickets.push_back(parse_ticket(line));
        }
    } else {
        std::cout << "Expected nearby tickets, got " << line << std::endl;
    }

    if (std::getline(is, line)) {
        std::cout << "Something went wrong before the end of input: " << line << std::endl;
    }

    return {std::move(constraints), std::move(your_ticket), std::move(nearby_tickets)};
}

bool is_valid(int value, constraint_t const& constraint) {
    return (constraint.first.low <= value && value <= constraint.first.high) ||
           (constraint.second.low <= value && value <= constraint.second.high);
}

bool is_any_valid(int value, constraints_t const& constraints) {
    return std::ranges::any_of(constraints | std::views::values, [value](constraint_t const& c) {
        return is_valid(value, c);
    });
}

int validate_ticket(ticket_t const& ticket, constraints_t const& constraints) {
    int invalid_sum = 0;
    for (int field : ticket) {
        if (!is_any_valid(field, constraints)) {
            invalid_sum += field;
        }
    }
    return invalid_sum;
}

int ticket_scanning_error_rate(std::vector<ticket_t> const& tickets, constraints_t const& constraints) {
    return tickets | std::views::transform([&](ticket_t const& t) { return validate_ticket(t, constraints); }) | accumulate(0);
}

void run() {
    problem_t const problem = parse(std::cin);

    std::cout << ticket_scanning_error_rate(problem.nearby_tickets, problem.constraints) << std::endl;
}
