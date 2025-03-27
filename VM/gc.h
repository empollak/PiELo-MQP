#ifndef GC_H
#define GC_H
#pragma once
#include "vm.h"
#include <vector>
#include <stack>
#include <map>
#include <string>

namespace PiELo {
    
  
    class GarbageCollector {
    public:

        // mark all reachable variables
        static void markRoots();
        
        // mark single var (marks nested references recursively)
        static void markVar(size_t closureIndex);

        // free unmarked variables and reset marks
        static void sweep();

        // run gc cycle
        static void collectGarbage();

        // might be useful for debugging (?)
        static size_t heapSize();
    };

}

#endif // GC_H