#include "tokenizer.h"
#include <iostream>

namespace PiELo {
    // Convert a string of characters into a list of tokens
    std::vector<std::string> tokenize(std::string str) {    
        std::vector<std::string> tokens;
        tokens.push_back("");
        std::string::iterator pos = str.begin();
        // Convert into list of tokens
        while (pos != str.end()) {
            std::cout << "analyzing char " << *pos << std::endl;
            if (*pos == '(' || *pos == ')') {
                // If it's an paren, push the paren
                std::cout << "pushed " << *pos << std::endl;
                if (*(--tokens.end()) == "") {
                    // If there was nothing else in the token, just place a paren in
                    (--tokens.end())->append(1, *pos);
                } else {
                    // Push the open or close paren
                    tokens.push_back(std::string(1, *pos));
                }
                // Push a new token for the next character
                tokens.push_back("");
            } else if (*pos == ' ') {
                // If it's a space, that means we're at a new token. 
                std::cout << "pushed new token" << std::endl;
                tokens.push_back("");
            } else {
                // Continuing in the previous token
                // (appends 1 copy of character *pos)
                (--tokens.end())->append(1, *pos);
                std::cout << "appended " << *pos << ". token now " << *(--tokens.end()) << std::endl;
            }
            pos++;
        }
        if (*--tokens.end() == "") tokens.erase(--tokens.end());
        return tokens;
    }
}
