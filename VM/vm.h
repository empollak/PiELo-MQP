#pragma once
#include <map>
#include <vector>
#include <string>
#include <stack>
#include "exceptions.h"
#include "instructionHandler.h"
#include <algorithm>

namespace PiELo {
    enum VMState {
        READY,
        DONE,
        ERROR
    };

    enum Type {
        NIL,
        PIELO_CLOSURE,
        C_CLOSURE,
        FLOAT,
        INT,
        NAME
    };

    Type stringToType(std::string s);

    // Temporary typedef for however we store code
    typedef size_t codePtr;

    struct Variable;

    typedef std::map<std::string, Variable> symbolTable;

    struct ClosureData {
        // Pointer to code, however we decide to store that
        codePtr codePointer;
        symbolTable localSymbolTable; // Should this be a pointer? Probably
        std::vector<std::string> argNames;
        std::vector<Type> argTypes;
        std::vector<std::string> dependencies;
    };

    struct opCodeInstructionOrArgument { // struct that helps handle the storing of values pushed by opcodes.
        enum {
            INSTRUCTION,
            FLOAT,
            INT,
            STRING,
            NIL,
            PIELO_CLOSURE,
            C_CLOSURE,
            NAME
        } type;
        union {
            Instruction asInstruction;
            float asFloat;
            int asInt;
            std::string* asString;
            ClosureData* asClosure;
        };
    
        opCodeInstructionOrArgument(int value_i) : type(INT) {asInt = value_i;}

        opCodeInstructionOrArgument(float f) : type(FLOAT) {asFloat = f;}

        opCodeInstructionOrArgument(Instruction instruction): type(INSTRUCTION) {asInstruction = instruction;}

        opCodeInstructionOrArgument(codePtr codePointer, std::vector<std::string> dependencies, std::vector<std::string> args, std::vector<PiELo::Type> argTypes) {
            type = PIELO_CLOSURE;
            asClosure = (ClosureData*) malloc(sizeof(ClosureData));
            asClosure->codePointer = codePointer;
            asClosure->argNames = args;
            asClosure->argTypes = argTypes;
        }

        opCodeInstructionOrArgument(ClosureData closureData) {
            type = PIELO_CLOSURE;
            asClosure = (ClosureData*) malloc(sizeof(ClosureData));
            *asClosure = closureData;
        }

        //opCodeInstructionOrArgument(const std::string& value_s) : type(STRING) {value.asString = new std::string(value);}
        
        opCodeInstructionOrArgument(const std::string s) : type(STRING) {
            asString = (std::string*) malloc(sizeof(std::string));
            *asString = s;
        }

        ~opCodeInstructionOrArgument() {
            if (type == STRING) {
                free(asString);
            } else if (type == PIELO_CLOSURE) {
                free(asClosure);
            }
        }
        
        std::string getTypeAsString() const {
            switch (type) {
                case NIL: return "NIL";
                case PIELO_CLOSURE: return "PIELO_CLOSURE";
                case C_CLOSURE: return "C_CLOSURE";
                case FLOAT: return "FLOAT";
                case INT: return "INT";
                case NAME: return "NAME";
                default: return "invalid type";
            }
        }
        
        float getFloatFromMemory() const {
            if (type != FLOAT) throw InvalidTypeAccessException("FLOAT", getTypeAsString());
            return asFloat;
        }

        int getIntFromMemory() const{
            if (type != INT) throw InvalidTypeAccessException("INT", getTypeAsString());
            return asInt;
        }

        // ClosureData* getClosureDataFromMemory() {
        //     if (type != PIELO_CLOSURE) throw InvalidTypeAccessException("PIELO_CLOSURE", getTypeAsString());
        //     return asClosure;
        // }
        
        std::string* getNameValueFromMemory() const{
            if (type != NAME) throw InvalidTypeAccessException("NAME", getTypeAsString());
            return asString;
        }
    };

    

    struct Tag {
        std::string tagName;
    };

    // All variables should be malloc'd? - i think we settled on no

    class Variable {
    private:
        union {
            int asInt;
            float asFloat;
            ClosureData* asClosure;
            std::string* asName;
        };
    public:
        Type type = NIL;
        bool changed = 0;
        // List of indices in the closure list
        std::vector<size_t> dependants;
        std::vector<Tag> tags;
        Variable* cachedValue; // Pointer because a struct definition can't include itself

        Variable() {
            cachedValue = (Variable*) malloc(sizeof(Variable));
        }

        Variable(std::string name) : type(NAME) {
            asName = (std::string*) malloc(sizeof(std::string));
            Variable();
        }

        Variable(int i): type(INT), asInt(i) {Variable();}

        Variable(float f) : type(FLOAT), asFloat(f) {Variable();}

        Variable(ClosureData c) : type(PIELO_CLOSURE), asClosure((ClosureData*) malloc(sizeof(ClosureData))) {Variable();}

        ~Variable() {
            free(cachedValue);
            if (type == NAME) free(asName);
            else if (type == PIELO_CLOSURE) free(asClosure);
        }

        std::string getTypeAsString() {
            switch (type) {
                case NIL: return "NIL";
                case PIELO_CLOSURE: return "PIELO_CLOSURE";
                case C_CLOSURE: return "C_CLOSURE";
                case FLOAT: return "FLOAT";
                case INT: return "INT";
                case NAME: return "NAME";
                default: return "invalid type";
            }
        }

        float getFloatValue() {
            if (type != FLOAT) throw InvalidTypeAccessException("FLOAT", getTypeAsString());
            return asFloat;
        }

        int getIntValue() {
            if (type != INT) throw InvalidTypeAccessException("INT", getTypeAsString());
            return asInt;
        }

        ClosureData* getClosureDataValue() {
            if (type != PIELO_CLOSURE) throw InvalidTypeAccessException("PIELO_CLOSURE", getTypeAsString());
            return asClosure;
        }
        
        std::string* getNameValue() {
            if (type != NAME) throw InvalidTypeAccessException("NAME", getTypeAsString());
            return asName;
        }
    };

    struct scopeData{
        symbolTable* scopeSymbolTable;
        codePtr codePointer;
    };

    
    extern std::vector<opCodeInstructionOrArgument> bytecode;
    extern codePtr programCounter;

    extern symbolTable taggedTable;
    // Variables which are at the top level but not tagged
    extern symbolTable globalSymbolTable;
    extern std::vector<ClosureData> closureList;
    extern std::stack<Variable> stack;


    extern std::stack<scopeData> returnAddrStack;
    
    extern std::map<std::string, Variable>* currentSymbolTable; 

    extern std::vector<Tag> robotTagList;

    extern VMState state;

    // Run one line of assembly code
    VMState step();

    // Take in a string filename. Expects a file of the format:
    // instruction <arguments> \n
    // Allows comments with #
    // Loads the instructions into the bytecode vector.
    VMState load(std::string filename);
}