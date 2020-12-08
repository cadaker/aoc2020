#include "intcode.hpp"
#include "input_helpers.hpp"
#include "range_helpers.hpp"

#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <ranges>

void run_until_repeat(std::vector<intcode::instruction> const& program, intcode::vm& vm) {
    std::unordered_set<size_t> visited;
    while (visited.count(vm.ip) == 0) {
        visited.insert(vm.ip);
        intcode::exec(vm, program.at(vm.ip));
    }
}

void run_until_end(std::vector<intcode::instruction> const& program, intcode::vm& vm) {
    long const end = static_cast<long>(program.size());
    while (vm.ip != end) {
        intcode::exec(vm, program.at(vm.ip));
    }
}

using jump_from_map = std::unordered_multimap<size_t, size_t>;

struct trace_point {
    long ip{};
    long patched_ip{-1};
};

intcode::instruction patched(long ip, intcode::instruction const& instr, long patched_ip) {
    if (patched_ip == ip) {
        if (instr.code == intcode::opcode::jmp) {
            return {intcode::opcode::nop, instr.argument};
        } else if(instr.code == intcode::opcode::nop) {
            return {intcode::opcode::jmp, instr.argument};
        }
    }

    return instr;
}

long trace_backwards(std::vector<intcode::instruction> const& program) {
    jump_from_map jump_from_jmp;
    jump_from_map jump_from_nop;
    for (long ip = 0; ip < static_cast<long>(program.size()); ++ip) {
        if (program[ip].code == intcode::opcode::jmp) {
            jump_from_jmp.emplace(ip + program[ip].argument, ip);
        } else if (program[ip].code == intcode::opcode::nop) {
            jump_from_nop.emplace(ip + program[ip].argument, ip);
        }
    }

    std::deque<trace_point> trace_queue{{static_cast<long>(program.size()), -1}};
    while (!trace_queue.empty()) {
        auto trace = trace_queue.front();
        trace_queue.pop_front();

        // Have we found our way to the start?
        if (trace.ip == 0 && trace.patched_ip != -1) {
            return trace.patched_ip;
        }
        // Find all ways to get to this point.
        auto fetch = [&](long ip) {
            return patched(ip, program.at(ip), trace.patched_ip);
        };
        // From the previous instruction.
        if (trace.ip > 0 && fetch(trace.ip - 1).code != intcode::opcode::jmp) {
            trace_queue.push_back({trace.ip-1, trace.patched_ip});
        }
        // By changing a previous jump to a nop
        if (trace.ip > 0 && trace.patched_ip == -1 && fetch(trace.ip - 1).code == intcode::opcode::jmp) {
            trace_queue.push_back({trace.ip-1, trace.ip-1});
        }

        for (auto const& p : pairseq(jump_from_jmp.equal_range(trace.ip))) {
            long const ip = p.second;
            // By jumping from somewhere
            if (ip != trace.patched_ip) {
                trace_queue.push_back({ip, trace.patched_ip});
            }
        }

        for (auto const& p : pairseq(jump_from_jmp.equal_range(trace.ip))) {
            long const ip = p.second;
            // By jumping from a changed nop
            if (ip == trace.patched_ip) {
                trace_queue.push_back({ip, trace.patched_ip});
            }
            // By changing a nop
            if (trace.patched_ip == -1) {
                trace_queue.push_back({ip, ip});
            }
        }
    }

    return -1;
}

void run() {
    std::vector<intcode::instruction> program;
    for (std::string const& line : input_lines(std::cin)) {
        auto inst = intcode::parse(line);
        if (inst.code == intcode::opcode::error) {
            std::cout << "Error: " << line << std::endl;
        } else {
            program.push_back(inst);
        }
    }

    intcode::vm vm{};
    run_until_repeat(program, vm);
    std::cout << vm.acc << std::endl;

    long const patched_ip = trace_backwards(program);

    if (0 <= patched_ip && patched_ip < static_cast<long>(program.size())) {
        auto patched_program = program;
        patched_program[patched_ip] = patched(patched_ip, patched_program[patched_ip], patched_ip);
        intcode::vm vm = {};
        run_until_end(patched_program, vm);
        std::cout << vm.acc << std::endl;
    }
}
