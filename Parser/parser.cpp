#include "parser.h"
#include "tokenizer.h"
#include <stdexcept>

namespace PiELo {


    Expression readFromTokens(std::vector<std::string>& tokens) {
        if (tokens.size() == 0) {
            throw std::runtime_error("Tokens had size 0");
        }
        // Pop off the front of the list
        std::cout << "readFromTokens got tokens: " << tokensToString(tokens) << std::endl;
        std::string token = *tokens.begin();
        tokens.erase(tokens.begin());
        std::cout << "after erase, readFromTokens got tokens: " << tokensToString(tokens) << std::endl;
        std::cout << "readFromTokens got token " << token << std::endl;
        if (token == "(") {
            Expression E;
            E.type = LIST;
            std::cout << "Got token (" << std::endl;
            // Add all tokens in this () pair, while nesting any nested () pairs
            while(*tokens.begin() != ")") {
                std::cout << "pushed to list: ";
                E.push_back(readFromTokens(tokens));
            }
            // Erase the leftover )
            tokens.erase(tokens.begin());
            return E;
        } else if (token == ")") {
            throw std::runtime_error("Unexpected )");
        } else {
            // Was not the start of a new procedure
            return atomize(token);
        }
    }

    Expression atomize(std::string token) {
        Expression result;
        try {
            if (token.find(".") != std::string::npos) {
                // There was a . so assume it's a float
                result.floatValue = std::stof(token);
                result.type = FLOAT;
                std::cout << "float " << result.floatValue << std::endl;
            } else {
                result.intValue = std::stoi(token);
                result.type = INT;
                std::cout << "int " << result.intValue << std::endl;
            }
        } catch (std::invalid_argument e) {
            result.type = SYMBOL;
            result.symbolValue = token;
            std::cout << "symbol " << result.symbolValue << std::endl;
        }
        return result;
    }

    std::string Expression::toString() {
        std::string retVal = "";
        switch (type) {
            case LIST:
                retVal += "[";
                for (Expression listExpression : listValue) {
                    retVal += listExpression.toString();
                }
                retVal += "]";
                break;
            case INT:
                retVal += std::to_string(intValue);
                break;
            case FLOAT:
                retVal += std::to_string(floatValue);
                break;
            case SYMBOL:
                retVal += "'";
                retVal += symbolValue;
                retVal += "'";
                break;
            case NIL:
                retVal += "NIL";
                break;
        }
        retVal += ", ";
        return retVal;
    }

    Expression parse(std::string program) {
        std::vector<std::string> tokens = tokenize(program);
        std::cout << "TOKENIZER OUTPUT:" << tokensToString(tokens) << std::endl;
        return readFromTokens(tokens);
    }
}