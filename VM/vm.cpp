#include "vm.h"
#include "parser.h"
#include "networking.h"
#include "robotFunctions.h"
#include "gc.h"

namespace PiELo {

    Type VM::stringToType(std::string s) {
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

        


    VM::VMState VM::load(std::string filename) {
        if (network.initNetworking(this) != 0) return VMState::ERROR;
        parser.load(filename, this);
        std::cout << "[pielo id " << robotID << "] bytecode size " << bytecode.size();
        std::cout << "bytecode top has type " << bytecode[0].getTypeAsString() << std::endl;
        state = READY;
        return VMState::READY;
    }

    VM::VMState VM::step() {
        // TODO: move the robot functions to a registered c function?
        // robot.updatePos();
        std::cout << "At pc " << programCounter << std::endl;
        handleInstruction(bytecode[programCounter]);
        network.checkForMessage();
        programCounter++;

        if (programCounter >= bytecode.size() || state == DONE) return VMState::DONE;
        return VMState::READY;
    }

    Variable* VM::findVariable(std::string name) {
        try {
            return &currentSymbolTable->at(name);
        } catch (...) {}

        try {
            return &globalSymbolTable.at(name);
        } catch (...) {}

        try {
            return &taggedTable.at(name);
        } catch (...) {}

        throw std::runtime_error("Could not find variable " + name);
    }

    // For now, c closures are limited to no arguments, no calling pielo closures, nothing!
    void VM::registerFunction(std::string name, funp f) {
        taggedTable[name] = f;
    }

    void VariableData::print(VM* vm) { 
        if(getType() == Type::NIL){
            std::cout << "nil";
        } else if(getType() == Type::INT){
            std::cout << "int " << asInt;
        } else if(getType() == Type::FLOAT){
            std::cout << "float " << asFloat;
        } else if (getType() == Type::PIELO_CLOSURE) {
            std::cout << "closure index: ";
            std::cout << asClosureIndex;
            std::cout << " cached value: ";
            vm->closureList[asClosureIndex].cachedValue.print(vm);
        }
    }

    VariableData Variable::nextIterValue() {
        ensureStig();
        VariableData data;
        if (iter != stigmergyData.end()) {
            // std::cout << " in here " << "with type " << iter->second.getTypeAsString() << std::endl;
            data = iter->second;
            iter++;
        }
        return data;
    }

    VariableData Variable::peekIterValue() {
        ensureStig();
        VariableData data;
        if (iter != stigmergyData.end()) {
            // std::cout << " peeking iter value " << "with type " << iter->second.getTypeAsString() << std::endl;
            data = iter->second;
        }
        return data;
    }

    void VM::ClosureMap::push_back(ClosureData& c) {
        #ifdef __DEBUG_INSTRUCTIONS__
            std::cout << "Inserting at index " << headOfList + 1 << " symbol table size " << c.localSymbolTable.size() << std::endl;
        #endif
        size_t insertLoc = headOfList++;
        this->insert(std::pair<size_t, ClosureData>(insertLoc, c));
        this->at(insertLoc).argNames = c.argNames;
        this->at(insertLoc).cachedValue = c.cachedValue;
        this->at(insertLoc).codePointer = c.codePointer;
        this->at(insertLoc).dependants = c.dependants;
        this->at(insertLoc).dependencies = c.dependencies;
        this->at(insertLoc).localSymbolTable = c.localSymbolTable;
        this->at(insertLoc).marked = c.marked;
    }

}
