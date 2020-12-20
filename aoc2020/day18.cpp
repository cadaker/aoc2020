#include "input_helpers.hpp"
#include <iostream>
#include <sstream>

enum class op_t {
    plus,
    times,
};

struct unprec_expr {
    std::vector<std::unique_ptr<unprec_expr>> terms;
    std::vector<op_t> ops;
    long value = 0;

    void push(op_t op) {
        ops.push_back(op);
    }

    void push(unprec_expr term) {
        terms.push_back(std::make_unique<unprec_expr>(std::move(term)));
    }
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

unprec_expr parse_expression(std::istream& is);

unprec_expr parse_term(std::istream& is) {
    eat_space(is);
    if (is.peek() == '(') {
        is.get();
        return parse_expression(is);
    } else {
        return {{}, {}, parse_value(is)};
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

unprec_expr parse_expression(std::istream& is) {
    eat_space(is);
    unprec_expr ret{};
    ret.push(parse_term(is));
    while (is) {
        eat_space(is);
        if (is.peek() == ')' || is.peek() == '\0') {
            is.get();
            eat_space(is);
            return ret;
        } else {
            op_t const op = read_op(is);
            eat_space(is);
            auto term = parse_term(is);
            ret.push(op);
            ret.push(std::move(term));
        }
        eat_space(is);
    }
    return ret;
}

unprec_expr parse_expression(std::string const& s) {
    std::istringstream iss(s);
    return parse_expression(iss);
}

long basic_eval(unprec_expr const& expr) {
    if (expr.terms.empty()) {
        return expr.value;
    } else {
        long value = basic_eval(*expr.terms.at(0));
        for (size_t i = 1; i < expr.terms.size(); ++i) {
            if (expr.ops.at(i-1) == op_t::plus) {
                value += basic_eval(*expr.terms.at(i));
            } else {
                value *= basic_eval(*expr.terms.at(i));
            }
        }
        return value;
    }
}

void run() {
    long sum = 0;
    for (std::string const& line : input_lines(std::cin)) {
        sum += basic_eval(parse_expression(line));
    }
    std::cout << sum << std::endl;
}
