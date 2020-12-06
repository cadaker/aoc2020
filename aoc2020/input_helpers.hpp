#pragma once

#include <iostream>
#include <string>
#include <iterator>
#include <ranges>
#include <memory>
#include <vector>

class input_line_iterator: public std::iterator<
        std::input_iterator_tag,
        std::string,
        std::ptrdiff_t,
        std::string const*,
        std::string const&
        > {
public:
    input_line_iterator()
            : _stream(nullptr)
    {}

    explicit input_line_iterator(std::istream& is)
            : _stream(&is)
    {
        read();
    }

    input_line_iterator(input_line_iterator const&) = default;
    input_line_iterator(input_line_iterator&&) = default;
    input_line_iterator& operator=(input_line_iterator const&) = default;
    input_line_iterator& operator=(input_line_iterator&&) = default;

    friend bool operator==(input_line_iterator const& x, input_line_iterator const& y) {
        return x._stream == y._stream;
    }

    friend bool operator!=(input_line_iterator const& x, input_line_iterator const& y) {
        return !(x == y);
    }

    reference operator*() const {
        return _line;
    };

    pointer operator->() {
        return &_line;
    }

    input_line_iterator& operator++() {
        read();
        return *this;
    }

    input_line_iterator operator++(int) {
        input_line_iterator ret = *this;
        read();
        return ret;
    }

private:
    void read() {
        if (_stream && !std::getline(*_stream, _line)) {
            _stream = nullptr;
        }
    }

    std::istream* _stream;
    std::string _line;
};

static_assert(std::input_iterator<input_line_iterator>);


class input_lines {
public:
    explicit input_lines(std::istream& is)
            : _stream(is)
    {}

    using iterator = input_line_iterator;

    [[nodiscard]] iterator begin() const {
        return iterator(_stream);
    }

    [[nodiscard]] iterator end() const {
        return iterator();
    }

private:
    std::istream& _stream;
};

static_assert(std::ranges::input_range<input_lines>);

template<>
constexpr bool std::ranges::enable_borrowed_range<input_lines> = true;

////////////////////////////////////////////////////////////////

inline std::vector<std::vector<std::string>> slurp_line_groups(std::istream& is) {
    std::vector<std::string> current_group;
    std::vector<std::vector<std::string>> result;
    for (std::string line : input_lines(is)) {
        if (!line.empty()) {
            current_group.push_back(std::move(line));
        } else {
            result.push_back(std::move(current_group));
            current_group = {};
        }
    }
    result.push_back(std::move(current_group));
    return result;
}
