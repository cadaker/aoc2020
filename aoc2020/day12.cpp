#include "input_helpers.hpp"
#include <iostream>
#include <unordered_map>
#include <string>

enum direction {
    east,
    north,
    west,
    south,
};

enum turn {
    left,
    right,
};

struct action {
    int instruction = 'F';
    int amount = 0;
};

struct ship {
    direction facing = direction::east;
    int x = 0;
    int y = 0;
};

direction turn_to(direction in, turn t, int amount) {
    static std::unordered_map<direction, direction> const left_turns = {
            {direction::east, direction::north},
            {direction::north, direction::west},
            {direction::west, direction::south},
            {direction::south, direction::east},
    };
    static std::unordered_map<direction, direction> const right_turns = {
            {direction::east, direction::south},
            {direction::north, direction::east},
            {direction::west, direction::north},
            {direction::south, direction::west},
    };
    if (amount == 90) {
        return (t == turn::left ? left_turns : right_turns).at(in);
    } else if (amount == 180) {
        return turn_to(turn_to(in, t, 90), t, 90);
    } else if (amount == 270) {
        return turn_to(turn_to(in, t, 180), t, 90);
    } else {
        return in;
    }
}

std::pair<int, int> steps(direction dir, int amount) {
    if (dir == direction::north) {
        return {0, amount};
    } else if (dir == direction::east) {
        return {amount, 0};
    } else if (dir == direction::south) {
        return {0, -amount};
    } else if (dir == direction::west) {
        return {-amount, 0};
    } else {
        return {0, 0};
    }
}

ship move(ship ship, int dx, int dy) {
    return {ship.facing, ship.x + dx, ship.y + dy};
}

ship execute(ship in, action action) {
    if (action.instruction == 'N') {
        auto [dx, dy] = steps(direction::north, action.amount);
        return move(in, dx, dy);
    } else if (action.instruction == 'S') {
        auto [dx, dy] = steps(direction::south, action.amount);
        return move(in, dx, dy);
    } else if (action.instruction == 'E') {
        auto [dx, dy] = steps(direction::east, action.amount);
        return move(in, dx, dy);
    } else if (action.instruction == 'W') {
        auto [dx, dy] = steps(direction::west, action.amount);
        return move(in, dx, dy);
    } else if (action.instruction == 'F') {
        auto [dx, dy] = steps(in.facing, action.amount);
        return move(in, dx, dy);
    } else if (action.instruction == 'L') {
        return {turn_to(in.facing, turn::left, action.amount), in.x, in.y};
    } else if (action.instruction == 'R') {
        return {turn_to(in.facing, turn::right, action.amount), in.x, in.y};
    } else {
        return in;
    }
}

action parse_action(std::string const& line) {
    if (!line.empty()) {
        return {line.front(), atoi(line.substr(1).c_str())};
    } else {
        return {};
    }
}

void run() {
    ship ship{};
    for (std::string const& line : input_lines(std::cin)) {
        ship = execute(ship, parse_action(line));
    }
    std::cout << std::abs(ship.x) + std::abs(ship.y) << std::endl;
}