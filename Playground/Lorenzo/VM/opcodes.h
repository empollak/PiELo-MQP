#ifndef OPCODES_H // include guards
#define OPCODES_H

#include <vector>

enum Opcode {
	PUSH,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	PRINT,
	HALT
};

class VM {
public:
	VM();
	void run(const std::vector<int>& code);

private:
	std::vector<int> stack;
	void push(int value);
	int pop();
};

#endif //OPCODES_H
