#include "vm.h"
#include "parser.h"
#include "networking.h"

namespace PiELo {
    Type stringToType(std::string s) {
        // Thanks to https://stackoverflow.com/questions/313970/how-to-convert-an-instance-of-stdstring-to-lower-case
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
        if (s == "nil") return NIL;
        else if (s == "pielo_closure") return PIELO_CLOSURE;
        else if (s == "c_closure") return C_CLOSURE;
        else if (s == "float") return FLOAT;
        else if (s == "int") return INT;
        else if (s == "name") return NAME;
        else throw InvalidTypeException();
    }

    std::vector<opCodeInstructionOrArgument> bytecode;
    std::vector<opCodeInstructionOrArgument> reactivityBytecodes;
    codePtr programCounter = 0;

    symbolTable taggedTable;
    // Variables which are at the top level but not tagged
    symbolTable globalSymbolTable;
    std::vector<ClosureData> closureList;
    std::vector<ClosureData> closureTemplates;
    std::stack<Variable> stack;


    std::stack<scopeData> returnAddrStack;
    
    std::map<std::string, Variable>* currentSymbolTable = &globalSymbolTable; 

    std::vector<Tag> robotTagList;

    size_t currentClosureIndex;

    VMState state;

    Parser parser;   


    VMState load(std::string filename) {
        parser.load(filename);
        if (initNetworking() != 0) return VMState::ERROR;
        return VMState::READY;
    }

    VMState step() {
        // std::cout << "now running instruction " << bytecode[programCounter].asInstruction << std::endl;
        // int x = bytecode[programCounter].asInstruction;
        // std::cout << "hello" << std::endl;
        handleInstruction(bytecode[programCounter]);
        programCounter++;
        if (programCounter >= bytecode.size() || state == DONE) return VMState::DONE;
        return VMState::READY;
    }

    Variable* findVariable(std::string name) {
        try {
            return &currentSymbolTable->at(name);
        } catch (...) {
            std::cout << "  findVariable: finding " << name << std::endl;
            return &taggedTable.at(name);
        }
    }
}