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
        if (input == "func") {
            std::string funcName;
            code >> funcName;
            printf("Creating function %s\n", funcName.c_str());
            std::streampos funcStart = code.tellg();
            push_to_heap(funcStart, funcName);
        }
    }

    // Reset the file back to start to run code
    code.clear();
    code.seekg(0, code.beg);
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
        } else if (input == "push") {
            std::string src;
            code >> src;
            std::cout << "Pushing value keyed by " << src << " to stack " << std::endl;
            push_from_heap(src);
        } else if (input == "addi") {
            std::cout << "Adding " << std::endl;
            add<int, int>();
        } else if (input == "call") {
            std::string funcName;
            code >> funcName;
            std::cout << "Calling " << funcName << std::endl;
            call(funcName);
        } else if (input == "ret") {
            std::cout << "Returning from function. " << std::endl;
            ret();
        } else if (input == "pop") {
            std::string dest;
            code >> dest;
            std::cout << "Popping top of stack to " << dest << std::endl;
            pop_store(dest);
        } else if (input == "exit") {
            std::cout << "Exiting.\n";
            break;
        } else {
            std::cout << "Invalid instruction " << input << std::endl;
        }
    }

    code.close();
}