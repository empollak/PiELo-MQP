#pragma once
// #include "../vm.h"
#include <string>

namespace PiELo {
    struct ClosureData;

    void defineClosure(std::string closureName, ClosureData closureData); 
}