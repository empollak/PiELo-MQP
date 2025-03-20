#include "gc.h"
#include "vm.h"
#include <iostream>
#include <algorithm>

namespace PiELo {
    
    std::vector<GCElement*> GarbageCollector::gcHeap;

    void GarbageCollector::regVar(Variable* var){
       GCElement* element = new GCElement;
       element->var = var;
       element->marked = false;
       gcHeap.push_back(element);
    }

    // mark one var
    void GarbageCollector::markVar(Variable* var){
        // look for entry in gcelement and mark it
        for (auto entry : gcHeap) {
            if (entry->var == var) {
                if (!entry->marked){
                    entry->marked = true;
                    // if closure, mark recursively local vars
                    if (var->getType() == PIELO_CLOSURE) {
                        // symbol table
                        ClosureData& closure = closureList[var->getClosureIndex()];
                        for (auto &pair: closure.localSymbolTable){
                            markVar(&pair.second);
                        }
                    }
                }
                break;
            }
        }
    }

    // mark all roots
    void GarbageCollector::markRoots(){
        // global symbol table
        for (auto &pair : globalSymbolTable){
            markVar(&pair.second);
        }

        // tagged table
        for (auto &pair : taggedTable){
            markVar(&pair.second);
        }

        // call stack
        std::stack<Variable> tempStack = stack;
        while (!tempStack.empty()) {
            markVar(&tempStack.top());
            tempStack.pop();
        }

        // local sym tables
        for (auto &closure : closureList) {
            for (auto &symPair : closure.localSymbolTable) {
                markVar(&symPair.second);
            }
        }

        // reactivity
        for (auto &op : reactivityBytecodes) {
            if (op.type == NAME) {
                Variable* var = findVariable(*op.asString);
                if (var) {
                    markVar(var);
                }
            }
            // other types go here
        }

    }

    // sweep through gc heap and free unmarked
    void GarbageCollector::sweep() {
        auto it = gcHeap.begin();
        while (it != gcHeap.end()) {
            if (!(*it)->marked) {
                // free var 
                delete (*it)->var;
                delete *it;
                it = gcHeap.erase(it);
            } else {
                // reset mark for next cycle
                (*it)->marked = false;
                ++it;
            }
        }
    }

    // run full gc cycle
    void GarbageCollector::collectGarbage() {
        std::cout << "GC: Starting Collection. Heap size = " << gcHeap.size() << std::endl;
        markRoots();
        sweep();
        std::cout << "GC  Collection complete. Heap size = " << gcHeap.size() << std::endl;
    }

    size_t GarbageCollector::heapSize(){
        return gcHeap.size();
    }

}
