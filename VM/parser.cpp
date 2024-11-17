#include "parser.h"
#include <iostream>
#include <stdexcept>

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
        {"push", &Parser::handlePush},
        {"store", &Parser::handleStore},
        {"tag", &Parser::handleTag},
        {"load", &Parser::handleLoad},
        {"push", &Parser::handlePush},
        {"pop", &Parser::handlePop},
        {"add", [&](const std::string&) { handleArithmetic(ADD); }},
        {"sub", [&](const std::string&) { handleArithmetic(SUB); }},
        {"print", [&](const std::string&) { handleSimple(PRINT); }},
        {"cmp", [&](const std::string&) { handleSimple(CMP); }},
        {"jmpg", [&](const std::string&) { handleJump(JMPG); }},
        {"jmpl", [&](const std::string&) { handleJump(JMPL); }},
        {"func", [&](const std::string&) { handleFunctionOrLabel("func"); }},
        {"label", [&](const std::string&) { handleFunctionOrLabel("label"); }},
        {"exit", [&](const std::string&) { handleSimple(EXIT); }},

    };
}

std::vector<opCodeInstructionOrArgument> Parser::load(){
    std::string instruction;

    while (file >> instruction) {
        auto it = instructionHandlers.find(instruction);
        if(it != instructionHandlers.end()) {
            // call appropriate handler
            (this -> *(it -> second))();
        }
        else {
            throwInvalidInstruction(instruction);
        }
    }

    return bytecode;
}

void Parser::handlePush() {
    std::string type;
    file >> type;

    if (type == "i") {
        bytecode.emplace_back(PUSHI);
        bytecode.emplace_back(parseNextInt());
    }
    else if (type == "f") {
        bytecode.emplace_back(PUSHF);
        bytecode.emplace_back(parseNextFloat());
    }
    else if (type == "s") {
        bytecode.emplace_back(PUSHS)
        bytecode.emaplace_back(parseNextString());
    }
    else {
        throwInvalidInstruction("push " + type);
    }

}

void Parser::handleStore() {
    std::string type;
    file >> type;
    
    if (type == "local") {
        bytecode.emplace_back(STORE_LOCAL);
    } 
    else if (type == "tagged") {
        bytecode.emplace_back(STORE_TAGGED);
        bytecode.emplace_back(parseNextString());  // tag name
    } 
    else {
        throwInvalidInstruction("store " + type);
    }
}


void Parser::handleTag() {
    std::string type;
    file >> type;

    if (type == "variable") {
        bytecode.emplace_back(TAG_VARIABLE);
        bytecode.emplace_back(parseNextString());  // variable name
        bytecode.emplace_back(parseNextString());  // tag name
    } else if (type == "robot") {
        bytecode.emplace_back(TAG_ROBOT);
        bytecode.emplace_back(parseNextString());  // tag name
    } else {
        throwInvalidInstruction("tag " + type);
    }
}

void Parser::handleLoad() {
    bytecode.emplace_back(LOAD_TO_STACK);
    bytecode.emplace_back(parseNextString());  // variable name
}

void Parser::handlePop() {
    bytecode.emplace_back(POP);
}

void Parser::handleArithmetic(const Instruction opcode) {
    bytecode.emplace_back(opcode);
}

void Parser::handleSimple(const Instruction opcode) {
    bytecode.emplace_back(opcode);
}

void Parser::handleFunctionOrLabel(const std::string& type) {
    int32_t position = bytecode.size();
    bytecode.emplace_back(type == "func" ? FUNCTION : LABEL);
    bytecode.emplace_back(parseNextString());  // name
}

void Parser::handleJump(const Instruction opcode) {
    bytecode.emplace_back(opcode);
    bytecode.emplace_back(parseNextString());  // label
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
