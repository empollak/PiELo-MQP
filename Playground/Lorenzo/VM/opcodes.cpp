#include "opcodes.h"
#include <iostream>
#include <stdexcept>

VM::VM() {}

void VM::push(int value)
{
	stack.push_back(value);
}

int VM::pop()
{
	if (stack.empty())
	{
		throw std::runtime_error("Stack underflow");
	}

	int value = stack.back();
	stack.pop_back();
	return value;
}

void VM::run(const std::vector<int> &code)
{
	size_t ip = 0; // instruction pointer
	while (ip < code.size())
	{
		Opcode op = static_cast<Opcode>(code[ip]);

		switch (op)
		{
		case PUSH:
		{
			int value = code[++ip];
			push(value);
			break;
		}
		case POP:
		{
			pop();
			break;
		}
		case ADD:
		{
			int b = pop();
			int a = pop();
			push(a + b);
			break;
		}

		case SUB:
		{
			int b = pop();
			int a = pop();
			push(a - b);
			break;
		}
		case MUL:
		{
			int b = pop();
			int a = pop();
			push(a * b);
			break;
		}
		case DIV:
		{
			int b = pop();
			int a = pop();
			if (b == 0)
			{
				throw std::runtime_error("Div by 0");
			}
			push(a / b);
			break;
		}
		case PRINT:
		{
			int value = pop();
			std::cout << value << std::endl;
			break;
		}
		case HALT:
		{
			return;
		}
		default:
			throw std::runtime_error("Unknown opcode");
		}

	++ip; // moves to next instruction
	
	}
}
