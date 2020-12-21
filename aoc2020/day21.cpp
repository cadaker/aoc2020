#include "input_helpers.hpp"
#include "range_helpers.hpp"
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <regex>

struct declaration_t {
    std::unordered_set<std::string> ingredients;
    std::unordered_set<std::string> allergens;
};

std::vector<std::string> get_tokens(std::string const& line, std::string const& pattern) {
    static std::regex const token_pattern(pattern);
    return {std::sregex_token_iterator(line.begin(), line.end(), token_pattern),
            std::sregex_token_iterator{}};
}

std::vector<declaration_t> parse(std::istream& is) {
    static std::regex const ingredients_pattern(R"(([a-z ]+) )");
    static std::regex const allergens_pattern(R"(\(contains ([a-z ,]+)\))");

    std::vector<declaration_t> declarations;

    for (std::string const& line : input_lines(is)) {
        std::smatch ingredients_match;
        std::smatch allergens_match;
        if (std::regex_search(line, ingredients_match, ingredients_pattern) &&
            std::regex_search(line, allergens_match, allergens_pattern)) {
            std::vector<std::string> ingredients = get_tokens(ingredients_match[1], "[a-z]+");
            std::vector<std::string> allergens = get_tokens(allergens_match[1], "[a-z]+");

            declarations.push_back(
                    {
                            {std::move_iterator(ingredients.begin()), std::move_iterator(ingredients.end())},
                            {std::move_iterator(allergens.begin()), std::move_iterator(allergens.end())}
                    });
        } else {
            std::cout << "Failed to match: " << line << std::endl;
        }
    }
    return declarations;
}

std::unordered_set<std::string> all_ingredients(std::vector<declaration_t> const& declarations) {
    std::unordered_set<std::string> ingredients;
    for (declaration_t const& d : declarations) {
        ingredients.insert(d.ingredients.begin(), d.ingredients.end());
    }
    return ingredients;
}

std::unordered_set<std::string> all_allergens(std::vector<declaration_t> const& declarations) {
    std::unordered_set<std::string> allergens;
    for (declaration_t const& d : declarations) {
        allergens.insert(d.allergens.begin(), d.allergens.end());
    }
    return allergens;
}

std::unordered_set<std::string> intersect(std::unordered_set<std::string> const& set1, std::unordered_set<std::string> const& set2) {
    std::unordered_set<std::string> ret;
    for (std::string const& elem : set1) {
        if (set2.find(elem) != set2.end()) {
            ret.insert(elem);
        }
    }
    return ret;
}

using allergen_to_ingredient_options = std::unordered_map<std::string, std::unordered_set<std::string>>;

allergen_to_ingredient_options ingredient_options(std::vector<declaration_t> const& declarations) {
    allergen_to_ingredient_options ret;
    for (std::string const& allergen: all_allergens(declarations)) {
        std::unordered_set<std::string> possible_ingredients = all_ingredients(declarations);
        for (declaration_t const& d : declarations) {
            if (d.allergens.contains(allergen)) {
                possible_ingredients = intersect(possible_ingredients, d.ingredients);
            }
        }
        ret[allergen] = std::move(possible_ingredients);
    }
    return ret;
}

std::unordered_set<std::string> safe_ingredients(
        allergen_to_ingredient_options const& ingredient_options,
        std::vector<declaration_t> const& declarations) {
    std::unordered_set<std::string> ret = all_ingredients(declarations);
    for (auto const& [allergen, ingredients] : ingredient_options) {
        for (std::string const& ingredient : ingredients) {
            ret.erase(ingredient);
        }
    }
    return ret;
}

size_t count_ingredients(std::unordered_set<std::string> const& ingredients, std::vector<declaration_t> const& declarations) {
    size_t count = 0;
    for (declaration_t const& d : declarations) {
        count += intersect(ingredients, d.ingredients).size();
    }
    return count;
}

std::unordered_map<std::string, std::string> reduce_options(allergen_to_ingredient_options options) {
    std::unordered_map<std::string, std::string> ret;
    auto only_one_ingredient = [](auto const&p) { return p.second.size() == 1; };
    while (true) {
        auto it = std::find_if(options.begin(), options.end(), only_one_ingredient);
        if (it != options.end()) {
            std::string const allergen = it->first;
            std::string const ingredient = *it->second.begin();

            ret[allergen] = ingredient;
            options.erase(allergen);
            for (auto& [_, ingredients] : options) {
                ingredients.erase(ingredient);
            }
        } else {
            break;
        }
    }
    return ret;
}

void run() {
    auto declarations = parse(std::cin);

    auto options = ingredient_options(declarations);

    std::cout << count_ingredients(safe_ingredients(options, declarations), declarations) << std::endl;

    auto reduced = reduce_options(options);

    bool first = true;
    for (auto const& [_, ingredient] : std::map<std::string, std::string>(reduced.begin(), reduced.end())) {
        std::cout << (first ? "" : ",") << ingredient;
        first = false;
    }
    std::cout << std::endl;
}
