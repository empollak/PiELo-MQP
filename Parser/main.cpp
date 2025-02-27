#include "tokenizer.h"
#include "parser.h"
#include "codegen.h"
#include "../VM/vm.h"
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    std::string input = "(if (+ -1 1) (if 0 1 -1) 3)";
    PiELo::Expression program = PiELo::parse(input);
    std::cout << program.toString() << std::endl;
    PiELo::codegenProgram(program, "code.txt");
}