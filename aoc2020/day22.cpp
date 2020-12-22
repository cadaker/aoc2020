#include <iostream>
#include <string>
#include <deque>
#include <iterator>
#include <vector>
#include <unordered_set>

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

using game_identifier = std::vector<char>;

game_identifier make_identifier(deck_t const& deck1, deck_t const& deck2) {
    game_identifier id(deck1.begin(), deck1.end());
    id.push_back(-1);
    id.insert(id.end(), deck2.begin(), deck2.end());
    return id;
}

struct game_identifier_hash {
    size_t operator()(game_identifier const& id) const {
        std::hash<char> h;
        size_t hash = 0;
        for (auto x : id) {
            hash = (hash << 1) ^ h(x);
        }
        return hash;
    }
};

enum class player {
    player_1,
    player_2,
};

player card_winner(int card1, int card2) {
    return card1 > card2 ? player::player_1 : player::player_2;
}

player recursive_game(deck_t& deck1, deck_t& deck2) {
    std::unordered_set<game_identifier, game_identifier_hash> played_matches;
    while (true) {
        if (!played_matches.insert(make_identifier(deck1, deck2)).second) {
            return player::player_1;
        } else if (deck1.empty()) {
            return player::player_2;
        } else if (deck2.empty()) {
            return player::player_1;
        } else {
            int const card1 = deck1.front();
            deck1.pop_front();
            int const card2 = deck2.front();
            deck2.pop_front();

            player winner{};
            if (deck1.size() >= card1 && deck2.size() >= card2) {
                deck_t deck1_copy(deck1.begin(), deck1.begin() + card1);
                deck_t deck2_copy(deck2.begin(), deck2.begin() + card2);
                winner = recursive_game(deck1_copy, deck2_copy);
            } else {
                winner = card_winner(card1, card2);
            }

            deck_t& winner_deck = winner == player::player_1 ? deck1 : deck2;
            winner_deck.push_back(winner == player::player_1 ? card1 : card2);
            winner_deck.push_back(winner == player::player_1 ? card2 : card1);
        }
    }
}

void run() {
    auto const [deck1_orig, deck2_orig] = parse(std::cin);

    {
        auto deck1 = deck1_orig;
        auto deck2 = deck2_orig;
        while (round(deck1, deck2)) {
        }
        deck_t const& winner = deck1.empty() ? deck2 : deck1;
        std::cout << score(winner) << std::endl;
    }

    {
        auto deck1 = deck1_orig;
        auto deck2 = deck2_orig;
        player const winner = recursive_game(deck1, deck2);
        std::cout << score(winner == player::player_1 ? deck1 : deck2) << std::endl;
    }
}
