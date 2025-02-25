#pragma once
#include <vector>
#include <string>

namespace PiELo {   
    enum tokenType {
        OPEN_PAREN,
        CLOSE_PAREN,
        IDENTIFIER, 

    };

    enum ExpressionType {
        LIST, ATOM, NIL
    };

    class Expression {
        public:
        
        Expression(){}
        virtual ~Expression(){}
        virtual ExpressionType getType() = 0;
    };

    class List : public Expression {
        public:
        std::vector<Expression> value;

        List(){
            value = {};
        }
    };

    class Atom : public Expression{
        private:


        public:
        enum {NUMBER, SYMBOL} type;
    };

    class Number : public Atom {
        enum {INT, FLOAT, NIL} type;
        union {
            int ivalue;
            float fvalue;
        };
    };

    class Symbol : public Atom {
        std::string value;

    };
}