#include "input_helpers.hpp"
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ranges>

using bag_contained_by_graph = std::unordered_multimap<std::string, std::string>;

void add_to_graph(bag_contained_by_graph& contained_by, std::string const& line) {
    static std::regex const prefix_regex(R"(^(.*?) bags contain)");
    static std::regex const contents_regex(R"((\d+) (.*?) bags?)");

    std::string container;
    std::smatch m;
    if (std::regex_search(line, m, prefix_regex) && m.size() > 1) {
        container = m[1];
    } else {
        return;
    }

    auto end = std::sregex_iterator{};
    for (auto it = std::sregex_iterator(line.begin(), line.end(), contents_regex); it != end; ++it) {
        auto const& match = *it;
        if (match.size() > 2) {
            contained_by.emplace(match[2], container);
        }
    }
}

void contained_by_dfs(bag_contained_by_graph const& contained_by, std::string const& node, std::unordered_set<std::string>& visited) {
    visited.insert(node);
    auto [begin, end] = contained_by.equal_range(node);
    for (auto const& [_, container] : std::ranges::subrange(begin, end)) {
        if (visited.count(container) == 0) {
            contained_by_dfs(contained_by, container, visited);
        }
    }
}

size_t nr_contained_by(bag_contained_by_graph const& contained_by, std::string const& start) {
    std::unordered_set<std::string> visited;
    contained_by_dfs(contained_by, start, visited);
    return visited.size() - 1;
}

void run() {
    bag_contained_by_graph bags;
    for (std::string const& line : input_lines(std::cin)) {
        add_to_graph(bags, line);
    }

    std::cout << nr_contained_by(bags, "shiny gold") << std::endl;
}
