#include "opcodes.h"
#include <iostream>
#include <stdexcept>

namespace PiELo_VM
{

	VM::VM() : ip(0)
	{
		init_handlers();
	}

	void VM::init_handlers()
	{

		opcode_handlers[PUSH] = [this]()
		{
			int value = code[ip++];
			push(Value(value));
		};

		opcode_handlers[POP] = [this]()
		{
			pop<Value>();
		};

		opcode_handlers[ADD] = [this]()
		{
			Value b = pop<Value>();
			Value a = pop<Value>();

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
				throw std::runtime_error("Type mismatch: INT or FLOAT expected");
			}
		};

		opcode_handlers[SUB] = [this]()
		{
			Value b = pop<Value>();
			Value a = pop<Value>();

			if (a.type == INT && b.type == INT)
			{
				push(Value(a.get_int() - b.get_int()));
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				push(Value(a.get_float() - b.get_float()));
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				push(Value(a.get_float() - static_cast<float>(b.get_int())));
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				push(Value(static_cast<float>(a.get_int()) - b.get_float()));
			}
			else
			{
				throw std::runtime_error("Type mismatch: INT or FLOAT expected");
			}
		};

		opcode_handlers[MUL] = [this]()
		{
			Value b = pop<Value>();
			Value a = pop<Value>();

			if (a.type == INT && b.type == INT)
			{
				push(Value(a.get_int() * b.get_int()));
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				push(Value(a.get_float() * b.get_float()));
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				push(Value(a.get_float() * b.get_float()));
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				push(Value(a.get_float() * b.get_float()));
			}
			else
			{
				throw std::runtime_error("Type mismatch: INT or FLOAT expected");
			}
		};

		opcode_handlers[DIV] = [this]()
		{
			Value b = pop<Value>();
			Value a = pop<Value>();

			if (b.type == INT && b.get_int() == 0)
			{
				throw std::runtime_error("Division by zero");
			}

			if (b.type == FLOAT && b.get_float() == 0.0f)
			{
				throw std::runtime_error("Division by zero");
			}

			if (a.type == INT && b.type == INT)
			{
				push(Value(a.get_int() / b.get_int()));
			}
			else if (a.type == FLOAT && b.type == FLOAT)
			{
				push(Value(a.get_float() / b.get_float()));
			}
			else if (a.type == FLOAT && b.type == INT)
			{
				push(Value(a.get_float() / static_cast<float>(b.get_int())));
			}
			else if (a.type == INT && b.type == FLOAT)
			{
				push(Value(static_cast<float>(a.get_int()) / b.get_float()));
			}
			else
			{
				throw std::runtime_error("Type mismatch: INT or FLOAT expected");
			}
		};

		opcode_handlers[PRINT] = [this]()
		{
			Value value = pop<Value>();

			switch (value.type)
			{
			case INT:
				std::cout << value.get_int() << std::endl;
				break;
			case BOOL:
				std::cout << (value.get_bool() ? "true" : "false") << std::endl;
				break;
			case FLOAT:
				std::cout << value.get_float() << std::endl;
				break;
			default:
				std::cout << "Unknown type" << std::endl;
			}
		};

		opcode_handlers[HALT] = [this]()
		{
			ip = code.size();
		};

		opcode_handlers[JMP] = [this]()
		{
			int target = code[ip++];
			ip = target;
		};

		opcode_handlers[JMP_IF_ZERO] = [this]()
		{
			Value zero_condition = pop<Value>();
			int target = code[ip++];

			if (zero_condition.type == INT && zero_condition.get_int() == 0)
			{
				ip = target;
			}
			else if (zero_condition.type == FLOAT && zero_condition.get_float() == 0.0f)
			{
				ip = target;
			}
			else if (zero_condition.type == BOOL && zero_condition.get_bool() == false)
			{
				ip = target;
			}
			else
			{
				// do nothing -> go next instruction
			}
		};
	}

	void VM::run(const std::vector<int> &prog_code)
	{
		code = prog_code;
		ip = 0;

		while (ip < code.size())
		{
			Opcode opcode = static_cast<Opcode>(code[ip++]);
			if (opcode_handlers.count(opcode))
			{
				opcode_handlers[opcode]();
			}
			else
			{
				throw std::runtime_error("Unknown opcode");
			}
		}
	}

	template <typename T>
	void VM::push(T value)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			stack.push_back(Value(value));
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			stack.push_back(Value(value));
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			stack.push_back(Value(value));
		}
		else
		{
			throw std::runtime_error("Unsupported PUSH type");
		}
	}

	template <typename T>
	T VM::pop()
	{
		if (stack.empty())
		{
			throw std::runtime_error("Stack underflow");
		}

		Value value = stack.back();
		stack.pop_back();

		if constexpr (std::is_same_v<T, int>)
		{
			return value.get_int();
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return value.get_float();
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return value.get_bool();
		}
		else if constexpr (std::is_same_v<T, Value>)
		{
			return value;
		}
		else
		{
			throw std::runtime_error("Unsupported POP type");
		}

		return T();
	}

	// friend function for printing vm state
	std::ostream &operator<<(std::ostream &os, const VM &vm)
	{
		os << "VM stack: [";
		for (const auto &value : vm.stack)
		{
			if (value.type == INT)
			{
				os << value.get_int() << " ";
			}
			else if (value.type == BOOL)
			{
				os << (value.get_bool() ? "true" : "false") << " ";
			}
			else if (value.type == FLOAT)
			{
				os << value.get_float() << " ";
			}
			else
			{
				os << "Unknown type ";
			}
			os << "]";
			return os;
		}
	}
} // end namespace