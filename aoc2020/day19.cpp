#include "range_helpers.hpp"
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <regex>

enum class rule_type {
    literal,
    recursive,
};

struct rule_t {
    rule_type type = rule_type::literal;
    int literal = 0;
    std::vector<std::vector<int>> choices;

    static rule_t make_literal(int literal) {
        return rule_t{rule_type::literal, literal};
    }

    static rule_t make_recursive(std::vector<std::vector<int>> choices) {
        return rule_t{rule_type::recursive, 0, std::move(choices)};
    }
};

using ruleset = std::unordered_map<int, rule_t>;

using pos_t = std::size_t;

std::unordered_set<pos_t> match_rule(ruleset const& rules, int current_rule, std::string const& text, pos_t start);

std::unordered_set<pos_t> match_rule(ruleset const& rules, int current_rule, std::string const& text, std::unordered_set<pos_t> const& starts) {
    std::unordered_set<pos_t> all_ends;
    for (pos_t start : starts) {
        auto ends = match_rule(rules, current_rule, text, start);
        all_ends.insert(ends.begin(), ends.end());
    }
    return all_ends;
}

// Returns the list of possible positions after a match
std::unordered_set<pos_t> match_rule(ruleset const& rules, int current_rule, std::string const& text, pos_t start) {
    rule_t const& rule = rules.at(current_rule);
    if (rule.type == rule_type::literal) {
        if (start < text.length() && text[start] == rule.literal) {
            return {start+1};
        } else {
            return {};
        }
    } else {
        std::unordered_set<pos_t> ret;
        for (auto const& choice : rule.choices) {
            std::unordered_set<pos_t> current_positions{start};
            for (int seq_index : choice) {
                current_positions = match_rule(rules, seq_index, text, current_positions);
            }
            ret.insert(current_positions.begin(), current_positions.end());
        }
        return ret;
    }
}

std::pair<ruleset, std::vector<std::string>> parse(std::istream& is) {
    ruleset rules;
    std::string line;
    while (std::getline(is, line) && !line.empty()) {
        static std::regex const literal_pattern(R"pat(([0-9]+): "(.)")pat");
        static std::regex const choice_pattern(R"([:|] ([0-9 ]+))");
        static std::regex const sequence_pattern(R"([0-9]+)");
        std::smatch m;
        if (std::regex_match(line, m, literal_pattern)) {
            int const index = atoi(m[1].str().c_str());
            int const literal = m[2].str()[0];
            rules[index] = rule_t::make_literal(literal);
        } else {
            int const index = atoi(line.c_str());
            auto iter = std::sregex_iterator(line.begin(), line.end(), choice_pattern);
            std::vector<std::vector<int>> rule_choices;
            for (std::smatch const& choice_m : pairseq(iter, std::sregex_iterator{})) {
                std::string choice = choice_m.str();
                std::vector<int> rule_sequence;
                auto seq_iter = std::sregex_iterator(choice.begin(), choice.end(), sequence_pattern);
                for (std::smatch const& seq_m : pairseq(seq_iter, std::sregex_iterator{})) {
                    rule_sequence.push_back(atoi(seq_m.str().c_str()));
                }
                rule_choices.push_back(std::move(rule_sequence));
            }
            rules[index] = rule_t::make_recursive(std::move(rule_choices));
        }
    }

    std::vector<std::string> messages;
    while (std::getline(is, line)) {
        messages.push_back(line);
    }
    return {std::move(rules), std::move(messages)};
}

void run() {
    auto [rules, messages] = parse(std::cin);

    size_t count = 0;
    for (std::string const& message : messages) {
        auto ends = match_rule(rules, 0, message, 0);
        if (ends.count(message.length()) > 0) {
            ++count;
        }
    }
    std::cout << count << std::endl;
}
