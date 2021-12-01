#include <iostream>
#include <string>

constexpr long N = 20201227;

long discrete_log(long base, long target, long n) {
    long value = 1;
    long iter = 0;
    while (value != target) {
        value = (value * base) % n;
        ++iter;
    }
    return iter;
}

std::pair<long, long> parse(std::istream& is) {
    std::string line;
    long value1 = 0;
    long value2 = 0;
    if (std::getline(is, line)) {
        value1 = atol(line.c_str());
    }
    if (std::getline(is, line)) {
        value2 = atol(line.c_str());
    }
    return {value1, value2};
}

long expmod(long base, long exponent, long n) {
    // Invariant: base**exponent * acc (mod n) is constant
    long acc = 1;
    while (true) {
        if (exponent == 0) {
            return acc;
        } else if (exponent % 2 == 0) {
            base = (base * base) % n;
            exponent /= 2;
        } else {
            --exponent;
            acc = (acc * base) % n;
        }
    }
}

void run() {
    auto const [value1, value2] = parse(std::cin);
    auto e = discrete_log(7, value1, N);
    std::cout << expmod(value2, e, N) << std::endl;
}
