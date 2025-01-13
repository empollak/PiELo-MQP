#include "storeLoad.h"
#include "../vm.h"
#include "../instructionHandler.h"
#include "../networking.h"
#include <sys/time.h>
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) std::cout << e;
#else
#define debugPrint(e)
#endif

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
        debugPrint("Beginning loadToStack " << std::endl);
        Variable* var = nullptr;

        // search local sym table
        #ifdef __DEBUG_INSTRUCTIONS__
            std::cout << "Searching current symbol table for " << varName << std::endl;
            std::cout << " symbol table has: " << std::endl;
            for (auto it : *currentSymbolTable) {
                std::cout << "  " << it.first << ":";
                it.second.print();
                std::cout << std::endl;
            }
        #endif

        auto local = currentSymbolTable -> find(varName);
        if (local != currentSymbolTable -> end()) {
            debugPrint("Found it! " << std::endl);
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
        #ifdef __DEBUG_INSTRUCTIONS__
        std::cout << "load result: ";
        std::cout << "Stack top: ";
        stack.top().print();
        std::cout << std::endl;
        #endif
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
    void recursivelyAddDependantsOfClosureToReturnAddrStack(size_t closureIndex, std::stack<scopeData> &dependants) {
        // Push the closure itself to the return address stack
        ClosureData* closure = &closureList[closureIndex];
        dependants.push((scopeData) {.scopeSymbolTable = &closure->localSymbolTable, .codePointer = closure->codePointer, .closureIndex = closureIndex});
        std::cout << closureIndex << ", ";
        // Push each of its dependants to the dependants stack, recursively
        for (size_t depIndex : closure->dependants) {
            recursivelyAddDependantsOfClosureToReturnAddrStack(depIndex, dependants);
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
            gettimeofday(&(var->lastUpdated), NULL);

            if (var->dependants.size() > 0) {
                // Store where we currently are
                std::cout << " " << varName << " has closure index dependants: ";
                returnAddrStack.push((scopeData){.scopeSymbolTable = currentSymbolTable, .codePointer = programCounter, .closureIndex = currentClosureIndex});
                
                std::stack<scopeData> dependants;
                for (int i = 0; i < var->dependants.size(); i++) {
                    // Push the info of each dependant closure and all of its dependants to the dependants stack
                    recursivelyAddDependantsOfClosureToReturnAddrStack(var->dependants[i], dependants);
                }

                // Reverse the order of the found dependants. This ensures that parent nodes are updated before their children
                size_t numDeps = dependants.size();
                for (size_t i = 0; i < numDeps; i++) {
                    returnAddrStack.push(dependants.top());
                    dependants.pop();
                }
                std::cout << std::endl;

                // Go to the first closure in the list
                programCounter = returnAddrStack.top().codePointer;
                currentSymbolTable = returnAddrStack.top().scopeSymbolTable;
                currentClosureIndex = returnAddrStack.top().closureIndex;
                returnAddrStack.pop();
            }
        } catch (...) {
            taggedTable[varName] = stack.top();
            taggedTable[varName].tags.push_back(Tag{tagName});
            gettimeofday(&taggedTable[varName].lastUpdated, NULL);
        }

        // Can assume that the variable now must exist in the tagged table
        broadcastVariable(varName, taggedTable[varName].getVariableData());
        stack.pop();
    }
}