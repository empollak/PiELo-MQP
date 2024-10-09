#include "opcodes.h"
#include <iostream>
#include <stdexcept>

VM::VM() {}

void VM::push(Value value)
{
	stack.push_back(value);
}

Value VM::pop()
{
	if (stack.empty())
	{
		throw std::runtime_error("Stack underflow");
	}

	Value value = stack.back();
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
			push(Value(value));
			break;
		}
		case POP:
		{
			pop();
			break;
		}
		case ADD:
		{
			Value b = pop();
			Value a = pop();
			if (a.type == INT && b.type == INT)
			{
				push(Value(a.get_int() + b.get_int()));
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				push(Value(a.get_float() + b.get_float()));
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				push(Value(a.get_float() + b.get_float()));
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				push(Value(a.get_float() + b.get_float()));
			}
			else
			{
				throw std::runtime_error("Type error in ADD");
			}
			break;
		}

		case SUB:
		{
			Value b = pop();
			Value a = pop();
			if (a.type == INT && b.type == INT)
			{
				push(Value(b.get_int() - a.get_int()));
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				push(Value(b.get_float() - a.get_float()));
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				push(Value(b.get_float() - a.get_float()));
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				push(Value(b.get_float() - a.get_float()));
			}
			else
			{
				throw std::runtime_error("Type error in SUB");
			}
			break;
		}
		case MUL:
		{
			Value b = pop();
			Value a = pop();
			if (a.type == INT && b.type == INT)
			{
				push(Value(b.get_int() * a.get_int()));
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				push(Value(b.get_float() * a.get_float()));
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				push(Value(b.get_float() * a.get_float()));
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				push(Value(b.get_float() * a.get_float()));
			}
			else
			{
				throw std::runtime_error("Type error in MUL");
			}
			break;
		}
		case DIV:
		{
			Value b = pop();
			Value a = pop();

			if (a.type == INT && b.type == INT)
			{
				if (b.get_int() == 0)
				{
					throw std::runtime_error("Div by 0");
				}
				else
				{
					push(Value(a.get_int() / b.get_int()));
				}
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				if (b.get_float() == 0.0f)
				{
					throw std::runtime_error("Div by 0");
				}
				else
				{
					push(Value(static_cast<float>(a.get_int()) / b.get_float()));
				}
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				if (b.get_int() == 0)
				{
					throw std::runtime_error("Div by 0");
				}
				else
				{
					push(Value(a.get_float() / b.get_float()));
				}
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				if (b.get_float() == 0.0f)
				{
					throw std::runtime_error("Div by 0");
				}
				else
				{
					push(Value(static_cast<float>(a.get_int()) / b.get_float()));
				}
			}
			break;
		}
		case PRINT:
		{
			Value value = pop();
			if (value.type == INT)
			{
				std::cout << value.get_int() << std::endl;
			}
			else if (value.type == FLOAT)
			{
				std::cout << value.get_float() << std::endl;
			}
			else if (value.type == BOOL)
			{
				std::cout << value.get_bool() << std::endl;
			}
			else
			{
				throw std::runtime_error("PRINT: unsupported type");
			}
		}
		case HALT:
		{
			return;
		}
		case JMP:
		{
			int target = code[ip++];
			ip = target;
			break;
		}
		case JMP_IF_ZERO:
		{
			Value zero_condition = pop();

			if (zero_condition.type == INT)
			{
				if (zero_condition.get_int() == 0)
				{
					int target = code[ip++];
					ip = target;
				}
				else
				{
					++ip;
				}
			}
			else if (zero_condition.type == BOOL)
			{
				if (!zero_condition.get_bool())
				{
					int target = code[ip++];
					ip = target;
				}
				else
				{
					++ip;
				}
			}
			else
			{
				throw std::runtime_error("Type error in JMP_IF_ZERO: Expected INT or BOOL");
			}
			break;
		}
		default:
			throw std::runtime_error("Unknown opcode");
		}

		++ip; // moves to next instruction
	}
}
