#include "input_views.hpp"
#include "range_helpers.hpp"
#include <string>
#include <vector>
#include <unordered_set>
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

std::unordered_set<int> join_answers(answers const& group) {
    std::unordered_set<int> answers;
    for (std::string const& answer : group) {
        for (char ch : answer) {
            answers.insert(ch);
        }
    }
    return answers;
}

void run() {
    auto const all_answers = parse_input(std::cin);
    std::cout << (all_answers
                  | std::views::transform(join_answers)
                  | std::views::transform([] (auto const& set) { return set.size(); })
                  | accumulate(0))
              << std::endl;
}
