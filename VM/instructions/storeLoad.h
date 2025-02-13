#pragma once
#include <string>
#include "../vm.h"

namespace PiELo {
    void storeLocal(std::string varName);
    void loadToStack(const std::string& varName);
    void tagVariable(const std::string& varName, const std::string& tagName);
    void tagRobot(const std::string& tagName);
    void storeTagged(const std::string& varName);
    void handleDependants(Variable& var);
    void storeStig(const std::string& varName);
    void pushNextElementOfStig(const std::string& varName);
    void isIterAtEnd(const std::string& varName);
    void resetIter(const std::string& varName);
}