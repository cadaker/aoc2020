#include "input_helpers.hpp"
#include <iostream>
#include <sstream>

enum class op_t {
    plus,
    times,
};

void eat_space(std::istream& is) {
    while (is) {
        if (isspace(is.peek())) {
            is.get();
        } else {
            break;
        }
    }
}

long parse_value(std::istream& is) {
    long value{};
    is >> value;
    return value;
}

long parse_expression(std::istream& is);

long parse_term(std::istream& is) {
    eat_space(is);
    if (is.peek() == '(') {
        is.get();
        return parse_expression(is);
    } else {
        return parse_value(is);
    }
}

op_t read_op(std::istream& is) {
    int const ch = is.peek();
    if (ch == '+') {
        is.get();
        return op_t::plus;
    } else if (ch == '*') {
        is.get();
        return op_t::times;
    } else {
        std::cout << "Error: expected op\n";
        return op_t::times;
    }
}

long parse_expression(std::istream& is) {
    eat_space(is);
    long value = parse_term(is);
    while (is) {
        eat_space(is);
        if (is.peek() == ')' || is.peek() == '\0') {
            is.get();
            eat_space(is);
            return value;
        } else {
            op_t const op = read_op(is);
            eat_space(is);
            long term = parse_term(is);
            value = op == op_t::plus ? (value + term) : (value * term);
        }
        eat_space(is);
    }
    return value;
}

long parse_expression(std::string const& s) {
    std::istringstream iss(s);
    return parse_expression(iss);
}

void run() {
    long sum = 0;
    for (std::string const& line : input_lines(std::cin)) {
        sum += parse_expression(line);
    }
    std::cout << sum << std::endl;
}
