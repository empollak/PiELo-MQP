#ifndef OPCODES_H // include guards
#define OPCODES_H

#include <vector>
#include "types.h"

enum Opcode
{
	PUSH,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	PRINT,
	HALT,
	JMP,
	JMP_IF_ZERO,
};

class VM
{
public:
	VM();
	void run(const std::vector<int> &code);

private:
	std::vector<Value> stack;
	void push(Value value);
	Value pop();
};

#endif // OPCODES_H
