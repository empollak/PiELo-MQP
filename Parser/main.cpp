#include "tokenizer.h"
#include "parser.h"
#include "codegen.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string input = "(+ x (+ (+ 2 3) 4.2))";
    PiELo::Expression program = PiELo::parse(input);
    std::cout << program.toString() << std::endl;
    PiELo::codegenProgram(program, "code.txt");
}