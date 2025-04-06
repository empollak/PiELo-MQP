#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <functional>
#include "instructionHandler.h"

namespace PiELo {
    class VM;
    struct opCodeInstructionOrArgument;

    typedef size_t codePtr;
    
    class Parser {
        public: 

            // load the program into memory and return the code vector
            void load(std::string filename, VM* vm);

        private:
            VM* vm;
            std::ifstream file;
            std::vector<opCodeInstructionOrArgument> code;
            std::unordered_map<std::string, std::function<void()>> instructionHandlers;
            std::unordered_map<std::string, codePtr> labelledLocations;

            // init handlers;
            void initHandlers();

            void handleDebugPrint();
            void handlePush();
            void handleStore();
            void handleTag();
            void handleLoad();
            void handlePop();
            void handleArithmetic(const Instruction opcode);
            void handleSimple(const Instruction opcode);
            void handleFunctionOrLabel(const std::string &type);
            void handleJump(const Instruction opcode);
            void handleDefineClosure();
            void handleCallC();

            void handleStigSize();

            void pushIntToCode();
            void pushFloatToCode();
            void pushStringToCode();
            
            int32_t parseNextInt();
            float parseNextFloat();
            std::string parseNextString();

            // error handling
            void throwInvalidInstruction(const std::string& instruction);
            std::vector<opCodeInstructionOrArgument> bytecode;
    };

}