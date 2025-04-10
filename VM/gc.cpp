#include "gc.h"
#include "vm.h"
#include <iostream>
#include <algorithm>
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) std::cout << e;
#else
#define debugPrint(e)
#endif

namespace PiELo {
    // mark one var
    void GarbageCollector::markVar(size_t closureIndex){
        // look for entry in gcelement and mark it
        ClosureData* closure;
        try {
            closure = &vm->closureList.at(closureIndex);
        } catch (std::out_of_range e) {
            throw std::out_of_range(std::string("closureList.at(") +  std::to_string(closureIndex) + ")");
        }
        closure->marked = true;

        if (closure->isTemplate) return;

        // symbol table
        for (auto &pair: closure->localSymbolTable){
            if (pair.second.getType() == PIELO_CLOSURE) {
                markVar(pair.second.getClosureIndex());
            }
        }
        
        
        // Mark all dependencies
        for (auto varName : closure->dependencies) {
            Variable* var = vm->findVariable(varName);
            if (var->getType() == PIELO_CLOSURE) markVar(var->getClosureIndex());
        }
        
        if (closure->cachedValue.getType() == PIELO_CLOSURE) markVar(closure->cachedValue.asClosureIndex);
    }

    // mark all roots
    void GarbageCollector::markRoots(){
        // Mark the closure we're currently in
        markVar(vm->currentClosureIndex);

        // global symbol table
        for (auto &pair : vm->globalSymbolTable){
            if (pair.second.getType() == PIELO_CLOSURE)
                markVar(pair.second.getClosureIndex());
        }

        // tagged table
        for (auto &pair : vm->taggedTable){
            if (pair.second.getType() == PIELO_CLOSURE)
                markVar(pair.second.getClosureIndex());
        }

        // call stack
        std::stack<Variable> tempStack = vm->stack;
        while (!tempStack.empty()) {
            if (tempStack.top().getType() == PIELO_CLOSURE) 
                markVar(tempStack.top().getClosureIndex());
            tempStack.pop();
        }

        // local sym tables
        for (auto &closurePair : vm->closureList) {
            if (closurePair.second.marked) {
                for (auto &localPair : closurePair.second.localSymbolTable) {
                    if (localPair.second.getType() == PIELO_CLOSURE)
                        markVar(localPair.second.getClosureIndex());
                }
            }
            
        }

        // Mark all closures in the return address stack
        std::stack<VM::scopeData> tempReturnStack = vm->returnAddrStack;
        while (!tempReturnStack.empty()) {
            markVar(tempReturnStack.top().closureIndex);
            tempReturnStack.pop();
        }
    }

    // sweep through gc heap and free unmarked
    void GarbageCollector::sweep() {

        // std::cout << "sweeping" << std::endl;
        for (std::map<size_t, ClosureData>::iterator it = vm->closureList.begin(); it != vm->closureList.end();) {
            // std::cout << "Looking at closure index " << it->first << std::endl;
            // Erase if not marked and move to the next item (erase does ++ for you)
            if (!it->second.marked) {
                // std::cout << "erasing closure index" << it->first << std::endl;
                it = vm->closureList.erase(it);
            }
            else {
                it->second.marked = false;
                it++;
            }
        }
    }

    // run full gc cycle
    void GarbageCollector::collectGarbage(VM* vmPtr) {
        vm = vmPtr;
        debugPrint("GC: Starting Collection. vm->closureList size = " << vmPtr->closureList.size() << std::endl)
        markRoots();
        sweep();
        debugPrint("GC  Collection complete. vm->closureList size = " << vmPtr->closureList.size() << std::endl)
    }

    size_t GarbageCollector::heapSize(){
        return vm->closureList.size();
    }

}
