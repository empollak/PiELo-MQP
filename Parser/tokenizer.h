#pragma once
#include <vector>
#include <string>

namespace PiELo {
    enum tokenType {
        OPEN_PAREN,
        CLOSE_PAREN,
        IDENTIFIER, 

    };

    class Expression {
        enum {LIST, ATOM} type;
    };

    class List : public Expression {
        std::vector<Expression> value;
    };

    class Atom : public Expression{
        private:


        public:
        enum {NUMBER, SYMBOL} type;
    };

    class Number : public Atom {
        union {
            int ivalue;
            float fvalue;
        };
    };

    class Symbol : public Atom {
        std::string value;

    };

    // Convert a string of characters into a list of tokens
    std::vector<std::string> tokenize(std::string str);
}