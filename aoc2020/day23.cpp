#include <iostream>
#include <string>
#include <ranges>
#include <vector>
#include <sstream>
#include <list>
#include <unordered_map>

class cups_t {
public:
    template<std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    requires std::convertible_to<std::iter_value_t<Iter>, int>
    cups_t(Iter begin, Sent end)
            : _cups(begin, end)
            , _current(_cups.begin())
    {
        for (auto it = _cups.begin(); it != _cups.end(); ++it) {
            _find_elem[*it] = it;
        }

    }

    void move() {
        int const current_val = *_current;
        int const total = static_cast<int>(_cups.size());
        auto const it1 = cyclic_next(_current);
        auto const it2 = cyclic_next(it1);
        auto const it3 = cyclic_next(it2);
        int const cup1 = *it1;
        int const cup2 = *it2;
        int const cup3 = *it3;

        _cups.erase(it1);
        _cups.erase(it2);
        _cups.erase(it3);

        int const next = next_current(current_val, cup1, cup2, cup3, total);
        auto next_it = _find_elem[next];
        _find_elem[cup3] = _cups.insert(std::next(next_it), cup3);
        _find_elem[cup2] = _cups.insert(std::next(next_it), cup2);
        _find_elem[cup1] = _cups.insert(std::next(next_it), cup1);

        _current = cyclic_next(_current);
    }

    [[nodiscard]] std::vector<int> cups() const {
        return {_cups.begin(), _cups.end()};
    }

private:
    std::list<int> _cups;
    std::unordered_map<int, std::list<int>::iterator> _find_elem;
    std::list<int>::iterator _current;

    [[nodiscard]] std::list<int>::iterator cyclic_next(std::list<int>::iterator it) {
        if (++it != _cups.end()) {
            return it;
        } else {
            return _cups.begin();
        }
    }

    [[nodiscard]] static int cyclic_prev(int x, int total) {
        if (x == 1) {
            return total;
        } else {
            return x - 1;
        }
    }

    [[nodiscard]] static int next_current(int current, int cup1, int cup2, int cup3, int total) {
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
