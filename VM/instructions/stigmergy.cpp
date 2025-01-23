#include "stigmergy.h"
#include "../vm.h"
#include <map>
#ifdef __DEBUG_INSTRUCTIONS__
#define debugPrint(e) std::cout << e;
#else
#define debugPrint(e)
#endif


namespace PiELo {
    void stigSize(std::string varName) {
        Variable* var = nullptr;
        auto tag = taggedTable.find(varName);
        if (tag != taggedTable.end()) {
            var = &(tag -> second);
        }
        
        if (!var){
            throw std::runtime_error("Variable not found (loadToStack): "+ varName);
        }

        if (!var->isStigmergy) {
            throw std::runtime_error("Attempted to find stig size of non-stig variable " + varName);
        }

        stack.push(var->getStigSize());
        debugPrint("Got stig size of " << varName << " as " << var->getStigSize() << std::endl);
    }
}