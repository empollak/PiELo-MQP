#include "storeLoad.h"
#include "../vm.h"
#include "../instructionHandler.h"

namespace PiELo {
 void storeLocal(std::string varName){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeLocal");
        }

        Variable var = stack.top();
        stack.pop();
        // TODO: fix to take name from instruction argument
        // std::string varName = *(var.getNameValue());
        (*currentSymbolTable)[varName] = var;
    }

    void loadToStack(const std::string& varName){
        std::cout << "Beginning loadToStack " << std::endl;
        Variable* var = nullptr;

        // search local sym table
        std::cout << "Searching current symbol table for " << varName << std::endl;
        std::cout << " symbol table has: " << std::endl;
        for (auto it : *currentSymbolTable) {
            std::cout << "  " << it.first << ":";
            it.second.print();
            std::cout << std::endl;
        }

        auto local = currentSymbolTable -> find(varName);
        if (local != currentSymbolTable -> end()) {
            std::cout << "Found it! " << std::endl;
            var = &(local -> second);
        }

        // search tagged table
        if (!var){
            auto tag = taggedTable.find(varName);
            if (tag != taggedTable.end()) {
                var = &(tag -> second);
            }
        }
        
        if (!var){
            throw std::runtime_error("Variable not found (loadToStack): "+ varName);
        }

        stack.push(*var);
        std::cout << "load result: ";
        std::cout << "Stack top: ";
        stack.top().print();
        std::cout << std::endl;
    }

    void tagVariable(const std::string& varName, const std::string& tagName) {
        Variable* var = nullptr;

        // search local sym table
        auto local = currentSymbolTable -> find(varName);
        if (local != currentSymbolTable -> end()){
            var = &(local -> second);
        }

        // search tagged table
        if (!var){
            auto tag = taggedTable.find(varName);
            if (tag != taggedTable.end()) {
                var = &(tag -> second);
            }
        }

        if (!var){
            throw std::runtime_error("Variable not found (tagVariable): " + varName);
        }

        // add tag
        var -> tags.push_back(Tag{tagName});
    }

    void tagRobot(const std::string& tagName) {
        
        // check if tag already exists
        for (const auto& tag : robotTagList){
            if (tag.tagName == tagName){
                // no need to add if it exists
                return;
            }
        }

        robotTagList.push_back(Tag{tagName});
    }

    // Takes in a closure index and does what it says it will do
    void recursivelyAddDependantsOfClosureToReturnAddrStack(size_t closureIndex) {
        // Push the closure itself to the return address stack
        ClosureData* closure = &closureList[closureIndex];
        returnAddrStack.push((scopeData) {.scopeSymbolTable = &closure->localSymbolTable, .codePointer = closure->codePointer});

        // Push each of its dependants to the return address stack, recursively
        for (size_t depIndex : closure->dependants) {
            recursivelyAddDependantsOfClosureToReturnAddrStack(depIndex);
        }
    }

    void storeTagged(const std::string& varName, const std::string& tagName){
        if (stack.empty()){
            throw std::runtime_error("Stack underflow: storeTagged");
        }
        std::cout << "Storing variable " << varName << " tag " << tagName << std::endl;
        try {
            // This will throw an error if varName is not found
            Variable* var = &taggedTable.at(varName);

            // TODO: Decide what to do with the tag
            var->tags.push_back(Tag{tagName});
            var->mutateValue(stack.top());

            // TODO: fix this
            if (var->dependants.size() > 0) {
                // Store where we currently are
                std::cout << " " << varName << " has closure index dependencies: ";
                returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter});
                for (int i = 0; i < var->dependants.size(); i++) {
                    std::cout << i << ", ";
                    // Push the info of each closure to the return address stack
                    // This way, we will return to them in order
                    recursivelyAddDependantsOfClosureToReturnAddrStack(var->dependants[i]);
                    // Variable currDepVar = var->dependants[i];
                    // ClosureData* currClosure = &closureList[currDepVar.getClosureIndex()];
                    // std::cout << "Pushing info of closure at index " << i << ": " << " code pointer: " << currClosure->codePointer << std::endl;
                    // returnAddrStack.push((scopeData) {.scopeSymbolTable = &currClosure->localSymbolTable, .codePointer = currClosure->codePointer});

                }
                std::cout << std::endl;
                // Go to the first closure in the list
                programCounter = returnAddrStack.top().codePointer;
                currentSymbolTable = returnAddrStack.top().scopeSymbolTable;
                returnAddrStack.pop();
            }
        } catch (...) {
            taggedTable[varName] = stack.top();
            taggedTable[varName].tags.push_back(Tag{tagName});
        }
        stack.pop();
    }
}