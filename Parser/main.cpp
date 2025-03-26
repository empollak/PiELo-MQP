#include "tokenizer.h"
#include "parser.h"
#include "codegen.h"
#include "../VM/vm.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unistd.h>

std::ifstream code;

int main(int argc, char** argv) {
    std::string programName = "program.txt";
    std::string outputFilename = "assembly.txt";
    int c;
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
            case 'o': 
                outputFilename = optarg;
                std::cout << "output filename: " << programName;
                break;
            case '?':
                if (optopt == 'o') {
                    std::cout << "Option -o requires an argument." << std::endl;
                } else {
                    std::cout << "Unknown option " << optopt << std::endl;
                }
                exit(1);
            default:
                exit(1);
        }
    }
    if (optind < argc) {
        std::cout << "Input filename: " << argv[optind] << std::endl;;
        programName = argv[optind];
        optind++;
    }
    while (optind < argc) {
        std::cout << "Too many arguments! Ignoring argument: " << argv[optind++] << std::endl;
    }
    PiELo::parseFile(programName, outputFilename);
}