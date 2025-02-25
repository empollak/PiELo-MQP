#include "parser.h"
#include "tokenizer.h"
#include <stdexcept>

namespace PiELo {
    Expression* readFromTokens(std::vector<std::string> tokens) {
        if (tokens.size() == 0) {
            throw std::runtime_error("Tokens had size 0");
        }
        // Pop off the front of the list
        std::string token = *tokens.erase(tokens.begin());
        if (token == "(") {
            List* L;
            // Add all tokens in this () pair, while nesting any nested () pairs
            while(*tokens.begin() != ")") {
                L->value.push_back(readFromTokens(tokens));
            }
            // Erase the leftover )
            tokens.erase(tokens.begin());
            return L;
        } else if (token == ")") {
            throw std::runtime_error("Unexpected )");
        } else {

        }
    }

    Atom atomize(std::string token) {
        Atom result;
        try {
            if (token.find(".") != std::string::npos) {
                // There was a . so assume it's a float

            }
        } catch (...) {

        }
    }

    Expression parse(std::string program) {
        return readFromTokens(tokenize(program));
    }
}