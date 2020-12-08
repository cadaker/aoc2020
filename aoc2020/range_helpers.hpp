#pragma once
#include <numeric>
#include <ranges>

template<class T, class Op = std::plus<>>
struct accumulate {
    explicit accumulate(T init, Op op = Op{})
            : _init(std::move(init))
            , _op(std::move(op))
    {}

    template<std::ranges::input_range Range>
    requires requires(accumulate& acc, Range&& range) {
        std::accumulate(std::ranges::begin(range), std::ranges::end(range), acc._init, acc._op);
    }
    auto operator()(Range&& range) const {
        return std::accumulate(std::ranges::begin(range), std::ranges::end(range), _init, _op);
    }
private:
    T _init;
    Op _op;
};

template<std::ranges::input_range Range, class T, class Op>
requires requires(Range&& range, accumulate<T, Op>&& fn) { fn(range); }
auto operator|(Range&& range, accumulate<T, Op>&& fn) {
    return fn(range);
}

////////////////////////////////////////////////////////////////

template<std::input_or_output_iterator Iter, std::sentinel_for<Iter> Sent>
struct pairseq {
    pairseq(std::pair<Iter, Sent> const& p)
            : iterator(p.first)
            , sentinel(p.second)
    {}
    Iter iterator;
    Sent sentinel;
};

template<class Iter, class Sent>
Iter begin(pairseq<Iter, Sent>& p) {
    return p.iterator;
}

template<class Iter, class Sent>
Iter begin(pairseq<Iter, Sent> const& p) {
    return p.iterator;
}

template<class Iter, class Sent>
Sent end(pairseq<Iter, Sent>& p) {
    return p.sentinel;
}

template<class Iter, class Sent>
Sent end(pairseq<Iter, Sent> const& p) {
    return p.sentinel;
}
