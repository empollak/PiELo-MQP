#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>

namespace PiELo {   
    enum tokenType {
        OPEN_PAREN,
        CLOSE_PAREN,
        IDENTIFIER, 

    };

    class Expression {
        public:
        enum ExpressionType {
            LIST, INT, FLOAT, SYMBOL, NIL
        };
        ExpressionType type = NIL;
        std::vector<Expression> listValue;  
        union {
            int intValue;
            float floatValue;
        };  
        std::string symbolValue;

        std::string typeToString() {
            switch (type) {
                case LIST:
                return "LIST";
                case INT:
                return "INT";
                case FLOAT:
                return "FLOAT";
                case SYMBOL:
                return "SYMBOL";
                case NIL:
                return "NIL";
                default:
                std::cout << "Unimplemented type to string for type " << type << std::endl;
                return "NIL";
            }
        }

        void push_back(Expression e) {
            if (type != LIST) throw std::runtime_error(std::string("Attempted to push_back to expression of type ") + typeToString());
            listValue.push_back(e);
        }

        std::string toString();

        Expression(std::string value) {
            type = SYMBOL;
            symbolValue = value;
        }
        
        Expression(){}
        ~Expression(){}
    };

    Expression parseString(std::string program);
    Expression atomize(std::string token);
    void parseFile(std::string inputFilename, std::string outputFilename);
}