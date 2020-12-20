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

long eval_with_precedence(std::vector<long> terms, std::vector<op_t> ops) {
    while (!ops.empty()) {
        long rhs = terms.back();
        terms.pop_back();
        op_t const op = ops.back();
        ops.pop_back();
        if (op == op_t::plus) {
            terms.back() += rhs;
        } else {
            return eval_with_precedence(std::move(terms), std::move(ops)) * rhs;
        }
    }
    return terms.back();
}

long eval_with_precedence(unprec_expr const& expr) {
    if (expr.terms.empty()) {
        return expr.value;
    } else {
        std::vector<long> terms;
        std::transform(expr.terms.begin(), expr.terms.end(), std::back_inserter(terms),
                       [](std::unique_ptr<unprec_expr> const& t) {
                           return eval_with_precedence(*t);
                       });
        std::vector<op_t> ops = expr.ops;
        return eval_with_precedence(std::move(terms), std::move(ops));
    }
}

void run() {
    long sum = 0;
    long sum_prec = 0;
    for (std::string const& line : input_lines(std::cin)) {
        auto expr = parse_expression(line);
        sum += basic_eval(expr);
        sum_prec += eval_with_precedence(expr);
    }
    std::cout << sum << std::endl;
    std::cout << sum_prec << std::endl;
}
