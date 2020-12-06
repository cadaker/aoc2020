#pragma once

#include <string>
#include <sstream>
#include <utility>

struct policy {
    int ch{};
    unsigned min{};
    unsigned max{};
};

std::pair<policy, std::string> parse_policy(std::string const& line);
