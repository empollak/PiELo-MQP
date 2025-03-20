#include "parser.h"
#include "tokenizer.h"
#include "codegen.h"
#include <stdexcept>
#include <sstream>
#include <vector>
#include <fstream>

namespace PiELo {


    Expression readFromTokens(std::vector<std::string>& tokens) {
        if (tokens.size() == 0) {
            throw std::runtime_error("Tokens had size 0");
        }
        // Pop off the front of the list
        std::string token = *tokens.begin();
        tokens.erase(tokens.begin());
        if (token == "(") {
            Expression E;
            E.type = Expression::LIST;
            // Add all tokens in this () pair, while nesting any nested () pairs
            while(*tokens.begin() != ")") {
                E.push_back(readFromTokens(tokens));
            }
            // Erase the leftover )
            if (tokens.size() == 0 || *tokens.begin() != ")") {
                throw std::runtime_error("Missing ')'. Rest of tokens: " + tokensToString(tokens));
            }
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
                result.type = Expression::FLOAT;
                std::cout << "float " << result.floatValue << std::endl;
            } else {
                result.intValue = std::stoi(token);
                result.type = Expression::INT;
                std::cout << "int " << result.intValue << std::endl;
            }
        } catch (std::invalid_argument e) {
            result.type = Expression::SYMBOL;
            result.symbolValue = token;
            std::cout << "symbol " << result.symbolValue << std::endl;
        }
        return result;
    }

    std::string Expression::toString() {
        std::string retVal = "";
        switch (type) {
            case Expression::LIST:
                retVal += "(";
                for (Expression listExpression : listValue) {
                    retVal += listExpression.toString();
                }
                // Erase the trailing space
                retVal.erase(retVal.end() - 1);
                retVal += ")";
                break;
            case Expression::INT:
                retVal += std::to_string(intValue);
                retVal += " ";
                break;
            case Expression::FLOAT:
                retVal += std::to_string(floatValue);
                retVal += " ";
                break;
            case Expression::SYMBOL:
                retVal += symbolValue;
                retVal += " ";
                break;
            case Expression::NIL:
                retVal += "NIL";
                break;
        }
        return retVal;
    }

    Expression parseString(std::string program) {
        std::vector<std::string> tokens = tokenize(program);
        std::cout << "TOKENIZER OUTPUT:" << tokensToString(tokens) << std::endl;
        return readFromTokens(tokens);
    }

    // Parse a file and codegen the contents
    void parseFile(std::string filename) {
        std::ifstream code;
        code.exceptions(std::ifstream::failbit);
        code.open(filename, std::fstream::in);
        std::stringstream buffer;
        buffer << code.rdbuf();
        std::string input = buffer.str();
        std::cout << "program " << input << std::endl;
        PiELo::Expression program = PiELo::parseString(input);
        std::cout << program.toString() << std::endl;
        PiELo::codegenProgram(program, "assembly.txt");
    }
}