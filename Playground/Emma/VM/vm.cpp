#include "vm.h"
#include "instructions.h"

std::map<std::string, std::vector<uint8_t>> heap;
std::stack<std::vector<uint8_t>> stack;
std::ifstream code;

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Please input a filename\n");
        exit(-1);
    }

    code.open(argv[1]);
    if (!code.is_open()) {
        printf("Error opening file %s\n", argv[1]);
        exit(-1);
    }

    std::string input;
    while (code >> input) {
        if (input == "print") {
            print_top();
        } else if (input == "printi") {
            print_with_type<int>();
        } else if (input == "pushi") {
            code >> input;
            int op = std::stoi(input);
            std::cout << "Pushing " << op << " to stack " << std::endl;
            push(op);
        } else if (input == "pushd") {
            code >> input;
            double op = std::stod(input);
            std::cout << "Pushing " << op << " to stack " << std::endl;
            push(op);
        } else if (input == "pushf") {
            code >> input;
            float op = std::stof(input);
            std::cout << "Pushing " << op << " to stack " << std::endl;
            push(op);
        } else if (input == "addi") {
            std::cout << "Adding " << std::endl;
            add<int, int>();
        }
    }

    code.close();
}