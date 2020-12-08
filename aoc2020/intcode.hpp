#pragma once

#include <string>
#include <regex>
#include <unordered_map>

namespace intcode {
    enum class opcode {
        error,
        acc,
        jmp,
        nop,
    };

    struct instruction {
        opcode code{};
        long argument{};
    };

    struct vm {
        long acc{};
        size_t ip{};
    };

    instruction parse(std::string const& line) {
        static std::regex const pattern(R"(([a-z]+) ([-+]\d+))");
        static std::unordered_map<std::string, opcode> lookup_opcode = {
                {"acc", opcode::acc},
                {"jmp", opcode::jmp},
                {"nop", opcode::nop},
        };
        std::smatch m;
        if (std::regex_match(line, m, pattern)) {
            std::string const argstr = m[2].str();
            char* endptr = nullptr;
            long const arg = std::strtol(argstr.c_str(), &endptr, 10);
            auto const it = lookup_opcode.find(m[1]);
            if (it != lookup_opcode.end() && endptr == argstr.c_str() + argstr.length()) {
                return {it->second, arg};
            }
        }
        return {opcode::error, 0};
    }

    bool exec(vm& vm, instruction const& instr) {
        switch (instr.code) {
            case opcode::acc:
                vm.acc += instr.argument;
                vm.ip++;
                return true;
            case opcode::jmp:
                vm.ip = static_cast<long>(vm.ip) + instr.argument;
                return true;
            case opcode::nop:
                vm.ip++;
                return true;
            default:
                return false;
        }
    }
}
