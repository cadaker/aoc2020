#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <ranges>
#include <cstring>

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

bool valid_year(std::string const& entry, int min, int max) {
    int y{};
    std::istringstream(entry) >> y;
    return entry.length() == 4 && min <= y && y <= max;
}

bool valid_hgt(std::string const& entry) {
    int h{};
    std::istringstream(entry) >> h;
    std::string const suffix = entry.substr(entry.size() - 2);
    return (suffix == "cm" && 150 <= h && h <= 193)
           || (suffix == "in" && 59 <= h && h <= 76);
}

bool valid_hcl(std::string const& entry) {
    return entry.length() == 7
           && entry[0] == '#'
           && std::all_of(entry.begin()+1,
                          entry.end(),
                          [](int ch) { return ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f'); });
}

bool valid_ecl(std::string const& entry) {
    std::vector<std::string> const alternatives = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
    return std::find(alternatives.begin(), alternatives.end(), entry) != alternatives.end();
}

bool valid_pid(std::string const& entry) {
    return entry.length() == 9 && std::all_of(entry.begin(), entry.end(), isdigit);
}

bool very_valid_passport(passport_info const& passport) {
    auto get = [&](char const* field) -> std::string {
        auto it = passport.find(field);
        return it != passport.end() ? it->second : std::string{};
    };
    return valid_passport(passport)
            && valid_year(get("byr"), 1920, 2002)
            && valid_year(get("iyr"), 2010, 2020)
            && valid_year(get("eyr"), 2020, 2030)
            && valid_hgt(get("hgt"))
            && valid_hcl(get("hcl"))
            && valid_ecl(get("ecl"))
            && valid_pid(get("pid"));
}

int main() {
    auto passports = parse_input(std::cin);

    auto validations = passports | std::views::transform(valid_passport);
    std::cout << std::count(validations.begin(), validations.end(), true) << std::endl;
    auto very_validations = passports | std::views::transform(very_valid_passport);
    std::cout << std::count(very_validations.begin(), very_validations.end(), true) << std::endl;
}
