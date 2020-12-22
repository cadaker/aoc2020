#include <iostream>
#include <string>
#include <deque>
#include <iterator>

using deck_t = std::deque<int>;

std::pair<deck_t, deck_t> parse(std::istream& is) {
    deck_t player1;
    deck_t player2;
    std::string line;
    while (std::getline(is, line) && line != "Player 1:") {}
    while (std::getline(is, line) && !line.empty()) {
        player1.push_back(std::atoi(line.c_str()));
    }
    while (std::getline(is, line) && line != "Player 2:") {}
    while (std::getline(is, line) && !line.empty()) {
        player2.push_back(std::atoi(line.c_str()));
    }
    return {std::move(player1), std::move(player2)};
}

bool round(deck_t& deck1, deck_t& deck2) {
    if (deck1.empty() || deck2.empty()) {
        return false;
    } else {
        int const card1 = deck1.front();
        deck1.pop_front();
        int const card2 = deck2.front();
        deck2.pop_front();
        deck_t& winner = (card1 > card2) ? deck1 : deck2;
        winner.push_back(std::max(card1, card2));
        winner.push_back(std::min(card1, card2));
        return true;
    }
}

size_t score(deck_t const& deck) {
    size_t i = deck.size();
    size_t score = 0;
    for (int card : deck) {
        score += i-- * static_cast<size_t>(card);
    }
    return score;
}

void run() {
    auto [deck1, deck2] = parse(std::cin);

    while (round(deck1, deck2)) {
    }
    deck_t const& winner = deck1.empty() ? deck2 : deck1;
    std::cout << score(winner) << std::endl;
}
