#include "vm.h"

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
    codePtr programCounter = 0;

    symbolTable taggedTable;
    // Variables which are at the top level but not tagged
    symbolTable globalSymbolTable;
    std::vector<ClosureData> closureList;
    std::stack<Variable> stack;


    std::stack<scopeData> returnAddrStack;
    
    std::map<std::string, Variable>* currentSymbolTable = &globalSymbolTable; 

    std::vector<Tag> robotTagList;

    VMState state;
}