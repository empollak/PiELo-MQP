#include "tokenizer.h"
#include "parser.h"
#include "codegen.h"
#include "../VM/vm.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

std::ifstream code;

int main(int argc, char** argv) {
    std::string programName = "program.txt";
    if (argc > 1) programName = argv[1];
    code.open(programName, std::fstream::in);
    std::stringstream buffer;
    buffer << code.rdbuf();
    std::string input = buffer.str();
    std::cout << "program " << input << std::endl;
    PiELo::Expression program = PiELo::parse(input);
    std::cout << program.toString() << std::endl;
    PiELo::codegenProgram(program, "assembly.txt");
}