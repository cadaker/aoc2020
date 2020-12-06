#include "input_helpers.hpp"
#include "range_helpers.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <ranges>

using answers = std::vector<std::string>;

std::vector<answers> parse_input(std::istream& is) {
    answers current_answers;
    std::vector<answers> result;
    for (std::string line : input_lines(is)) {
        if (!line.empty()) {
            current_answers.push_back(std::move(line));
        } else {
            result.push_back(std::move(current_answers));
            current_answers = {};
        }
    }
    result.push_back(std::move(current_answers));
    return result;
}

std::unordered_map<int, int> count_answers(answers const& group) {
    std::unordered_map<int, int> answers;
    for (std::string const& answer : group) {
        for (char ch : answer) {
            answers[ch]++;
        }
    }
    return answers;
}

std::unordered_map<int, int> count_intersection(answers const& group) {
    std::unordered_map<int, int> answers = count_answers(group);
    std::erase_if(answers, [&](std::pair<int, int> const& p) { return p.second != group.size(); });
    return answers;
}

void run() {
    auto const all_answers = parse_input(std::cin);
    std::cout << (all_answers
                  | std::views::transform(count_answers)
                  | std::views::transform([] (auto const& counts) { return counts.size(); })
                  | accumulate(0))
              << std::endl;
    std::cout << (all_answers
                  | std::views::transform(count_intersection)
                  | std::views::transform([] (auto const& counts) { return counts.size(); })
                  | accumulate(0))
              << std::endl;
}
