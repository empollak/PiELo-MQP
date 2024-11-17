#include "parser.h"
#include <iostream>
#include <stdexcept>
#include <functional>
#include <numeric>

using namespace PiELo;

Parser::Parser(const std::string& filename) {
    file.open(filename);
    if (!file.is_open()){
        throw std::runtime_error("Error opening file: "+ filename);
    }
    initHandlers();
}

void Parser::initHandlers() {
     instructionHandlers = {
        {"store", [&]() {Parser::handleStore();}},
        {"tag", [&]() {Parser::handleTag();}},
        {"load", [&]() {Parser::handleLoad();}},
        {"push", [&]() {Parser::handlePush();}},
        {"pop", [&]() {handlePop();}},
        {"add", [&]() { handleArithmetic(ADD); }},
        {"sub", [&]() { handleArithmetic(SUB); }},
        {"print", [&]() { handleSimple(PRINT); }},
        {"eql", [&]() { handleSimple(EQL); }},
        {"neql", [&]() { handleSimple(NEQL); }},
        {"gt", [&]() { handleSimple(GT); }},
        {"gte", [&]() { handleSimple(GTE); }},
        {"lt", [&]() { handleSimple(LT); }},
        {"lte", [&]() { handleSimple(LTE); }},
        {"jmp", [&]() { handleJump(JMP); }},
        {"jmp_if_zero", [&]() { handleJump(JMP_IF_ZERO); }},
        {"jmp_if_not_zero", [&]() { handleJump(JMP_IF_NOT_ZERO); }},
        {"func", [&]() { handleFunctionOrLabel("func"); }},
        {"label", [&]() { handleFunctionOrLabel("label"); }},
        {"end", [&]() { handleSimple(END); }},
        {"define_closure", [&]() { handleDefineClosure(); }},
        {"call_closure", [&]() { handleSimple(CALL_CLOSURE); }},
        {"ret_from_closure", [&]() {handleSimple(RET_FROM_CLOSURE);}},
        {"#", [&]() { file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }},
    };
}

void Parser::load(){
    std::string instruction;

    while (file >> instruction) {
        auto it = instructionHandlers.find(instruction);
        if(it != instructionHandlers.end()) {
            // call appropriate handler
            instructionHandlers[instruction]();
        }
        else {
            throwInvalidInstruction(instruction);
        }
    }

    // return bytecode;
}

void Parser::handlePush() {
    std::string type;
    file >> type;

    if (type == "i") {
        bytecode.push_back((int) PUSHI);
        bytecode.push_back(parseNextInt());
    }
    else if (type == "f") {
        bytecode.push_back((int) PUSHF);
        bytecode.push_back(parseNextFloat());
    }
    else if (type == "s") {
        bytecode.push_back((int) PUSHS);
        bytecode.push_back(parseNextString());
    }
    else {
        throwInvalidInstruction("push " + type);
    }

}

void Parser::handleStore() {
    std::string type;
    file >> type;
    
    if (type == "local") {
        bytecode.push_back(STORE_LOCAL);
    } 
    else if (type == "tagged") {
        bytecode.push_back(STORE_TAGGED);
        bytecode.push_back(parseNextString());  // tag name
    } 
    else {
        throwInvalidInstruction("store " + type);
    }
}


void Parser::handleTag() {
    std::string type;
    file >> type;

    if (type == "variable") {
        bytecode.push_back(TAG_VARIABLE);
        bytecode.push_back(parseNextString());  // variable name
        bytecode.push_back(parseNextString());  // tag name
    } else if (type == "robot") {
        bytecode.push_back(TAG_ROBOT);
        bytecode.push_back(parseNextString());  // tag name
    } else {
        throwInvalidInstruction("tag " + type);
    }
}

void Parser::handleLoad() {
    bytecode.push_back(LOAD_TO_STACK);
    bytecode.push_back(parseNextString());  // variable name
}

void Parser::handlePop() {
    bytecode.push_back(POP);
}

void Parser::handleArithmetic(const Instruction opcode) {
    bytecode.push_back(opcode);
}

void Parser::handleSimple(const Instruction opcode) {
    bytecode.push_back(opcode);
}

void Parser::handleFunctionOrLabel(const std::string& type) {
    int32_t position = bytecode.size();
    labelledLocations[parseNextString()] = position;
}

void Parser::handleJump(const Instruction opcode) {
    bytecode.push_back(opcode);
    std::string name = parseNextString();
    auto it = labelledLocations.find(name);
    if (it != labelledLocations.end()) {
        bytecode.push_back((int) it->second);  // label
    } else {
        throw std::runtime_error("Invalid label name " + name);
    }
}

void Parser::handleDefineClosure() {
    bytecode.push_back(DEFINE_CLOSURE);
    ClosureData closure;
    std::string name = parseNextString();
    bytecode.push_back(name);
    int numArgs = parseNextInt();
    for (int i = 0; i < numArgs; i++) {
        closure.argTypes[i] = stringToType(parseNextString());
        closure.argNames[i] = parseNextString();
    }
    int numDependencies = parseNextInt();
    for (int i = 0; i < numDependencies; i++) {
        closure.dependencies[i] = parseNextString();
    }
    bytecode.push_back(closure);
}

int32_t Parser::parseNextInt() {
    int32_t value;
    file >> value;
    return value;
}

float Parser::parseNextFloat() {
    float value;
    file >> value;
    return value;
}

std::string Parser::parseNextString() {
    std::string value;
    file >> value;
    return value;
}

void Parser::throwInvalidInstruction(const std::string& instruction) {
    throw std::runtime_error("Invalid instruction: " + instruction);
}
