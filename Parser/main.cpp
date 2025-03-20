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
    PiELo::parseFile(programName);
}