#include "instructions.h"
#include "parser.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <fstream>

using namespace PiELo;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please input a filename\n");
        exit(-1);
    }

    load(argv[1]);
    while (step() > 0);
}