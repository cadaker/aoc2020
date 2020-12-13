#pragma once
#include <ranges>

inline long extended_euclidian(long r0, long r1, long& inv0, long& inv1) {
    long s0 = 1;
    long s1 = 0;
    long t0 = 0;
    long t1 = 1;
    while (r1 != 0) {
        long const r = r0 % r1;
        long const q = r0 / r1;
        long const s = s0 - q * s1;
        long const t = t0 - q * t1;
        r0 = r1;
        r1 = r;
        s0 = s1;
        s1 = s;
        t0 = t1;
        t1 = t;
    }
    inv0 = s0;
    inv1 = t0;
    return r0;
}

namespace detail {
    inline long huge_mult(long x, long y, long mod, long acc = 0) {
        // Invariant: x*y + acc == answer
        if (x == 0) {
            return acc;
        } else if (x < 0) {
            return huge_mult(-x, -y, mod, -acc);
        } else if (x % 2 == 0) {
            return huge_mult(x / 2, (y*2) % mod, mod, acc);
        } else {
            return huge_mult(x-1, y, mod, (acc + y) % mod);
        }
    }
}

template<std::ranges::input_range RemRng, std::ranges::input_range ModRng>
        requires std::is_convertible_v<std::ranges::range_value_t<RemRng>, long> &&
                 std::is_convertible_v<std::ranges::range_value_t<ModRng>, long>
inline long chinese_remainder(RemRng&& remainders, ModRng&& moduli) {
    auto reduce = [](long x, long m) {
        return ((x % m) + m) % m;
    };

    long x = 0;
    long mod = 1;
    auto remainder_it = begin(remainders);
    auto modulus_it = begin(moduli);
    for (; remainder_it != end(remainders) && modulus_it != end(moduli); ++remainder_it, ++modulus_it) {
        long const r = *remainder_it;
        long const m = *modulus_it;
        long inv_mod{}, inv_m{};
        extended_euclidian(mod, m, inv_mod, inv_m);

        x = reduce(x + detail::huge_mult(r - x, inv_mod * mod, mod*m), mod*m);
        mod = m * mod;
    }
    return x;
}
