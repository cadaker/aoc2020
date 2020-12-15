#include "input_helpers.hpp"
#include <iostream>
#include <regex>
#include <unordered_map>
#include <numeric>

enum class oper {
    mask,
    mem,
};

struct instr {
    oper op;
    unsigned long arg1;
    unsigned long arg2;

    static instr mask(unsigned long use, unsigned long override) {
        return instr{oper::mask, use, override};
    }

    static instr mem(unsigned long dest, unsigned long value) {
        return instr{oper::mem, dest, value};
    }
};

struct mask {
    unsigned long use;
    unsigned long override;
};

mask split_mask(std::string const& mask_str) {
    unsigned long use = 0;
    unsigned long override = 0;
    for (char ch : mask_str) {
        use <<= 1;
        override <<= 1;
        if (ch == '0') {
            use |= 1;
            override |= 0;
        } else if (ch == '1') {
            use |= 1;
            override |= 1;
        }
    }
    return {use, override};
}

instr parse(std::string const& line) {
    static std::regex const mask_pattern(R"(mask = ([10X]+))");
    static std::regex const mem_pattern(R"(mem\[([0-9]+)\] = ([0-9]+))");
    std::smatch m;
    if (std::regex_match(line, m, mask_pattern)) {
        mask mask = split_mask(m[1].str());
        return instr::mask(mask.use, mask.override);
    } else if(std::regex_match(line, m, mem_pattern)) {
        unsigned long dest = atol(m[1].str().c_str());
        unsigned long value = atol(m[2].str().c_str());
        return instr::mem(dest, value);
    } else {
        return {};
    }
}

void apply(instr const& instr, std::unordered_map<unsigned long, unsigned long>& memory, mask& current_mask) {
    if (instr.op == oper::mask) {
        current_mask.use = instr.arg1;
        current_mask.override = instr.arg2;
    } else {
        memory[instr.arg1] = (instr.arg2 & ~current_mask.use) | (current_mask.override & current_mask.use);
    }
}

std::vector<unsigned long> explode_mask(unsigned long use, size_t index) {
    if (index >= 36) {
        return {0UL};
    } else if (use & (1UL << index)) {
        return explode_mask(use & ~(1UL << index), index+1);
    } else {
        auto exploded = explode_mask(use, index + 1);
        size_t const size = exploded.size();
        exploded.reserve(size * 2);
        for (size_t i = 0; i < size; ++i) {
            exploded.push_back(exploded[i] | (1UL << index));
        }
        return exploded;
    }
}

void apply_version2(instr const& instr, std::unordered_map<unsigned long, unsigned long>& memory, mask& current_mask) {
    if (instr.op == oper::mask) {
        current_mask.use = instr.arg1;
        current_mask.override = instr.arg2;
    } else {
        unsigned long base_address = (instr.arg1 & current_mask.use) | current_mask.override;
        for (unsigned long mask : explode_mask(current_mask.use, 0)) {
            memory[(base_address | mask)] = instr.arg2;
        }
    }
}

void run() {
    std::unordered_map<unsigned long, unsigned long> memory, memory2;
    mask current_mask{}, current_mask2{};
    for (std::string const& line : input_lines(std::cin)) {
        instr const instr = parse(line);

        apply(instr, memory, current_mask);
        apply_version2(instr, memory2, current_mask2);
    }

    std::cout << (std::accumulate(memory.begin(), memory.end(), 0UL, [](unsigned long sum, auto& p) {
        return sum + p.second;
    }))
              << std::endl;

    std::cout << (std::accumulate(memory2.begin(), memory2.end(), 0UL, [](unsigned long sum, auto& p) {
        return sum + p.second;
    }))
              << std::endl;
}
