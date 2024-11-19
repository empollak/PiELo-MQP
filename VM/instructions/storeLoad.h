#include <string>

namespace PiELo {
    void storeLocal(std::string varName);
    void loadToStack(const std::string& varName);
    void tagVariable(const std::string& varName, const std::string& tagName);
    void tagRobot(const std::string& tagName);
    void storeTagged(const std::string& varName, const std::string& tagName);
    
}