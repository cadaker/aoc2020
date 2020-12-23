#include <iostream>
#include <string>
#include <ranges>
#include <vector>
#include <sstream>

class cups_t {
public:
    template<std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    requires std::convertible_to<std::iter_value_t<Iter>, int>
    cups_t(Iter begin, Sent end)
            : _cups(begin, end)
    {
    }

    void move() {
        int const current_val = _cups.at(_current);
        int const total = static_cast<int>(_cups.size());
        size_t const ix1 = cyclic_next(_current);
        size_t const ix2 = cyclic_next(ix1);
        size_t const ix3 = cyclic_next(ix2);
        int const cup1 = _cups.at(ix1);
        int const cup2 = _cups.at(ix2);
        int const cup3 = _cups.at(ix3);

        auto it1 = std::remove(_cups.begin(), _cups.end(), cup1);
        auto it2 = std::remove(_cups.begin(), it1, cup2);
        auto it3 = std::remove(_cups.begin(), it2, cup3);
        _cups.erase(it3, _cups.end());

        int const next = next_current(current_val, cup1, cup2, cup3, total);
        auto next_it = std::find(_cups.begin(), _cups.end(), next);
        _cups.insert(std::next(next_it), {cup1, cup2, cup3});

        _current = cyclic_next(std::find(_cups.begin(), _cups.end(), current_val) - _cups.begin());
    }

    [[nodiscard]] std::vector<int> cups() const {
        return _cups;
    }

private:
    std::vector<int> _cups;
    size_t _current = 0;

    [[nodiscard]] size_t cyclic_next(size_t ix) const {
        if (ix + 1 < _cups.size()) {
            return ix + 1;
        } else {
            return 0;
        }
    }

    [[nodiscard]] static int cyclic_prev(int x, int total) {
        if (x == 1) {
            return total;
        } else {
            return x - 1;
        }
    }

    [[nodiscard]] int next_current(int current, int cup1, int cup2, int cup3, int total) const {
        int next = cyclic_prev(current, total);
        while (next == cup1 || next == cup2 || next == cup3) {
            next = cyclic_prev(next, total);
        }
        return next;
    }
};

std::vector<int> parse(std::istream& is) {
    std::string line;
    std::vector<int> ret;
    if (std::getline(is, line)) {
        for (char ch : line) {
            ret.push_back(ch - '0');
        }
    }
    if (!ret.empty()) {
        return ret;
    } else {
        return {1};
    }
}

std::string order_after_1(std::vector<int> const& cups) {
    std::ostringstream oss;
    auto it = std::find(cups.begin(), cups.end(), 1);
    if (it != cups.end()) {
        std::copy(std::next(it), cups.end(), std::ostream_iterator<int>(oss));
        std::copy(cups.begin(), it, std::ostream_iterator<int>(oss));
    }
    return oss.str();
}

void run() {
    auto input = parse(std::cin);
    cups_t cups(input.begin(), input.end());

    for (int i = 0; i < 100; ++i) {
        cups.move();
    }
    std::cout << order_after_1(cups.cups()) << std::endl;
}
