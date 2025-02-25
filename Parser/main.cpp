#include "tokenizer.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string input = "(set x (+ a b))";
    std::vector<std::string> tokens = PiELo::tokenize(input);
    std::cout << "[";
    for (std::vector<std::string>::iterator tokenIter = tokens.begin(); tokenIter != tokens.end(); tokenIter++) {
        std::cout << "'" << *tokenIter << "', ";
    }
    std::cout << "]" << std::endl;
}