#include "input_helpers.hpp"
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ranges>

using bag_contained_by_graph = std::unordered_multimap<std::string, std::string>;
using bag_contains_graph = std::unordered_multimap<std::string, std::pair<int, std::string>>;

void add_to_graphs(bag_contained_by_graph& contained_by, bag_contains_graph& contains, std::string const& line) {
    static std::regex const prefix_regex(R"(^(.*?) bags contain)");
    static std::regex const contents_regex(R"((\d+) (.*?) bags?)");

    std::string container;
    std::smatch m;
    if (std::regex_search(line, m, prefix_regex) && m.size() > 1) {
        container = m[1];
    } else {
        return;
    }

    for (auto const& match : std::ranges::subrange(std::sregex_iterator(line.begin(), line.end(), contents_regex), std::sregex_iterator{})) {
        if (match.size() > 2) {
            contained_by.emplace(match[2], container);
            int const count = std::atoi(match[1].str().c_str());
            contains.emplace(container, std::pair(count, match[2]));
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

size_t contains_dfs(bag_contains_graph const& contains, std::string const& node) {
    size_t total_contents = 1;
    auto [begin, end] = contains.equal_range(node);
    for (auto const& [_, contents] : std::ranges::subrange(begin, end)) {
        int const count = contents.first;
        std::string const& type = contents.second;
        total_contents += count * contains_dfs(contains, type);
    }
    return total_contents;
}

size_t nr_contains(bag_contains_graph const& contains, std::string const& start) {
    return contains_dfs(contains, start) - 1;
}

void run() {
    bag_contained_by_graph contained_by;
    bag_contains_graph contains;
    for (std::string const& line : input_lines(std::cin)) {
        add_to_graphs(contained_by, contains, line);
    }

    std::cout << nr_contained_by(contained_by, "shiny gold") << std::endl;

    std::cout << nr_contains(contains, "shiny gold") << std::endl;
}
