#pragma once

#include <string>
#include <sstream>
#include <utility>

struct policy {
    int ch{};
    unsigned min{};
    unsigned max{};
};

inline std::pair<policy, std::string> parse_policy(std::string const& line) {
    std::istringstream iss(line);
    policy policy;
    if (iss >> policy.min &&
        iss.get() == '-' &&
        iss >> policy.max &&
        iss.get() == ' ' &&
        std::isalnum(policy.ch = iss.get()) &&
        iss.get() == ':' &&
        iss.get() == ' ')
    {
        std::string password;
        std::getline(iss, password);
        return {policy, password};
    } else {
        return {};
    }
}
