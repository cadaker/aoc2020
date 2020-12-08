#include "intcode.hpp"
#include "input_helpers.hpp"

#include <iostream>
#include <unordered_set>

void run_until_repeat(std::vector<intcode::instruction> const& program, intcode::vm& vm) {
    std::unordered_set<size_t> visited;
    while (visited.count(vm.ip) == 0) {
        visited.insert(vm.ip);
        intcode::exec(vm, program.at(vm.ip));
    }
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
}
