#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <functional>

template<class T>
class grid {
public:
    grid()
            : grid(0, 0)
    {}

    grid(size_t rows, size_t cols)
            : _cols(cols)
            , _data(rows*cols, {})
    {
    }

    [[nodiscard]] bool is_inside(size_t r, size_t c) const {
        return r < rows() && c < cols();
    }

    [[nodiscard]] size_t cols() const {
        return _cols;
    }

    [[nodiscard]] size_t rows() const {
        return _cols > 0 ? _data.size() / _cols : 0;
    }

    T& operator()(size_t r, size_t c) {
        return _data.at(index(r, c));
    }

    T const& operator()(size_t r, size_t c) const {
        return _data.at(index(r, c));
    }

    explicit operator bool() const {
        return !_data.empty();
    }

    using iterator = T*;
    using const_iterator = T const*;

    [[nodiscard]] size_t size() const {
        return _data.size();
    }

    iterator begin() {
        return &_data[0];
    }

    const_iterator begin() const {
        return &_data[0];
    }

    iterator end() {
        return begin() + _data.size();
    }

    const_iterator end() const {
        return begin() + _data.size();
    }

    iterator row_begin(size_t r) {
        if (is_inside(r, 0)) {
            return begin() + index(r, 0);
        } else {
            return end();
        }
    }

    const_iterator row_begin(size_t r) const {
        if (is_inside(r, 0)) {
            return begin() + index(r, 0);
        } else {
            return end();
        }
    }

    iterator row_end(size_t r) {
        if (is_inside(r, 0)) {
            return begin() + index(r+1, 0);
        } else {
            return end();
        }
    }

    const_iterator row_end(size_t r) const {
        if (is_inside(r, 0)) {
            return begin() + index(r+1, 0);
        } else {
            return end();
        }
    }

private:
    size_t index(size_t r, size_t c) const {
        return r * _cols + c;
    }

    size_t _cols;
    std::vector<T> _data;
};

template<class T>
class grid_builder {
public:
    grid_builder() = default;

    void push_back(T x) {
        _elems.push_back(std::move(x));
    }

    void finish_row() {
        if (_cols == 0) {
            _cols = _elems.size();
        } else if (_elems.size() % _cols != 0) {
            std::cout << "Warning: unbalanced rows!";
        }
    }

    grid<T> build() {
        grid<T> ret(_cols > 0 ? _elems.size() / _cols : 0, _cols);
        std::copy(_elems.begin(), _elems.end(), ret.begin());
        return ret;
    }

private:
    size_t _cols = 0;
    std::vector<T> _elems;
};

template<class T, class F>
struct grid_formatter {
    grid_formatter(grid<T> const& grid, F func)
            : _grid(grid)
            , _func(std::move(func))
    {}

    grid<T> const& _grid;
    F _func;
};

template<class T, class F>
std::ostream& operator<<(std::ostream& os, grid_formatter<T, F>&& formatter) {
    for (size_t r = 0; r < formatter._grid.rows(); ++r) {
        for (size_t c = 0; c < formatter._grid.cols(); ++c) {
            os << (c == 0 ? "" : " ") << formatter._func(formatter._grid(r, c));
        }
        os << "\n";
    }
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, grid<T> const& grid) {
    return os << grid_formatter(grid, [](T const& x) -> T const& { return x; });
}

template<class T>
std::vector<T> row(grid<T> const& grid, size_t r) {
    return std::vector<T>(grid.row_begin(r), grid.row_end(r));
}

template<class T>
std::vector<T> col(grid<T> const& grid, size_t c) {
    std::vector<T> ret;
    if (grid.is_inside(0, c)) {
        for (auto it = grid.begin(); it != grid.end(); it += grid.cols()) {
            ret.push_back(*(it + c));
        }
    }
    return ret;
}

template<class T>
grid<T> join_horiz(grid<T> const& g1, grid<T> const& g2) {
    if (g1.rows() == g2.rows()) {
        grid<T> ret(g1.rows(), g1.cols() + g2.cols());
        for (size_t row = 0; row < ret.rows(); ++row) {
            std::copy(g1.row_begin(row), g1.row_end(row), ret.row_begin(row));
            std::copy(g2.row_begin(row), g2.row_end(row), ret.row_begin(row) + g1.cols());
        }
        return ret;
    } else {
        return {0, 0};
    }
}

template<class T>
grid<T> join_vert(grid<T> const& g1, grid<T> const& g2) {
    if (g1.cols() == g2.cols()) {
        grid<T> ret(g1.rows() + g2.rows(), g1.cols());
        for (size_t row = 0; row < g1.rows(); ++row) {
            std::copy(g1.row_begin(row), g1.row_end(row), ret.row_begin(row));
        }
        for (size_t row = 0; row < g2.rows(); ++row) {
            std::copy(g2.row_begin(row), g2.row_end(row), ret.row_begin(row + g1.rows()));
        }
        return ret;
    } else {
        return {0, 0};
    }
}

template<class T>
grid<T> subgrid(grid<T> const& g, size_t r_begin, size_t r_end, size_t c_begin, size_t c_end) {
    if (r_begin <= r_end && c_begin <= c_end && r_end <= g.rows() && c_end <= g.cols()) {
        grid<T> ret(r_end - r_begin, c_end - c_begin);
        for (size_t r = r_begin; r < r_end; ++r) {
            std::copy(g.row_begin(r) + c_begin, g.row_begin(r) + c_end, ret.row_begin(r - r_begin));
        }
        return ret;
    } else {
        return {};
    }
}

template<class T>
grid<T> rotate_ccw(grid<T> const& g) {
    grid<T> ret(g.cols(), g.rows());
    for (size_t r = 0; r < g.rows(); ++r) {
        for (size_t c = 0; c < g.cols(); ++c) {
            ret(g.cols() - 1 - c, r) = g(r, c);
        }
    }
    return ret;
}

template<class T>
grid<T> mirror_horiz(grid<T> const& g) {
    grid<T> ret = g;
    for (size_t r = 0; r < ret.rows(); ++r) {
        std::reverse(ret.row_begin(r), ret.row_end(r));
    }
    return ret;
}

template<class T>
grid<T> mirror_vert(grid<T> const& g) {
    grid<T> ret(g.rows(), g.cols());
    for (size_t r = 0; r < ret.rows(); ++r) {
        std::copy(g.row_begin(r), g.row_end(r), ret.row_begin(ret.rows() - 1 - r));
    }
    return ret;
}
