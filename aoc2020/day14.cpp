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

void run() {
    std::unordered_map<unsigned long, unsigned long> memory;
    mask current_mask{};
    for (std::string const& line : input_lines(std::cin)) {
        instr const instr = parse(line);
        apply(instr, memory, current_mask);
    }

    std::cout << (std::accumulate(memory.begin(), memory.end(), 0UL, [](unsigned long sum, auto& p) {
        return sum + p.second;
    }))
              << std::endl;
}
