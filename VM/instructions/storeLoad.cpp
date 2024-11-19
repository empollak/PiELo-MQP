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
        printf("huh whuh\n");
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
                for (int i = 0; i < var->dependants.size(); i++) {
                    // Run each dependant closure
                    std::cout << "Rerunning closure at index " << var->dependants[i] << std::endl;
                    bytecode.insert(bytecode.begin() + programCounter + (1 + 3*i), RERUN_CLOSURE);
                    bytecode.insert(bytecode.begin() + programCounter + (2 + 3*i), (int) var->dependants[i]);
                    // Pop the closure index that ret_from_closure will push
                    bytecode.insert(bytecode.begin() + programCounter + (3 + 3*i), POP);
                }
            }
        } catch (...) {
            taggedTable[varName] = stack.top();
            taggedTable[varName].tags.push_back(Tag{tagName});
        }
        stack.pop();
    }
}