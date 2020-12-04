#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <ranges>

using passport_info = std::unordered_map<std::string, std::string>;

std::pair<std::string, std::string> parse_entry(std::string const& entry) {
    auto const pos = entry.find(':');
    return {entry.substr(0, pos), entry.substr(pos+1)};
}

std::vector<passport_info> parse_input(std::istream& is) {
    std::vector<passport_info> infos;
    passport_info current_info;
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty()) {
            infos.push_back(std::move(current_info));
            current_info = {};
        } else {
            std::istringstream iss(line);
            for (auto [key, value] : std::ranges::istream_view<std::string>(iss) | std::views::transform(parse_entry)) {
                current_info.emplace(key, value);
            }
        }
    }
    if (!current_info.empty()) {
        infos.push_back(current_info);
    }
    return infos;
}

bool valid_passport(passport_info const& passport) {
    static std::vector const required_fields = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
    auto matches = required_fields | std::views::transform([&](char const* field) { return passport.count(field) > 0; });
    return std::all_of(matches.begin(), matches.end(), std::identity{});
}

int main() {
    auto passports = parse_input(std::cin);

    auto validations = passports | std::views::transform(valid_passport);
    std::cout << std::count(validations.begin(), validations.end(), true) << std::endl;
}
