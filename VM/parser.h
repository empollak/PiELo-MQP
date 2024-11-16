#pragma once
#include "vm.h"
#include "parser.h"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

namespace PiELo {
    
    class Parser {
        public: 
            explicit Parser(const std:: string& filename);

            // load the program into memory and return the code vector
            std::vector<opCodeInstructionOrArgument> load();

        private:
            std::ifstream file;
            std::vector<opCodeInstructionOrArgument> code;
            std::unordered_map<std::string, void (Parser::*)()> instructionHandlers;

            // init handlers;
            void initHandlers();

            void handlePush();
            void handleStore();
            void handleTag();
            void handleLoad();
            void handleJump();
            void handlePop();
            void handleArithmetic(const Instruction opcode);
            void handleSimple(const Instruction opcode);
            void handleFunctionOrLabel(Instruction opcode);
            void handleJump(const Instruction opcode);

            void pushIntToCode();
            void pushFloatToCode();
            void pushStringToCode();
            
            int32_t parseNextInt();
            float parseNextFloat();
            std::string parseNextString();

            // error handling
            void throwInvalidInstruction(const std::string& instruction);
    };

}