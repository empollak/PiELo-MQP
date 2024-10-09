#include "opcodes.h"
#include <iostream>
#include <stdexcept>

namespace PiELo_VM
{

	VM::VM() : ip(0), status(VMStatus::OK) // init with OK status
	{
		init_handlers();
	}

	VMStatus VM::step()
	{
		if (ip >= code.size())
		{
			status = VMStatus::DONE;
			return status;
		}

		Opcode opcode = static_cast<Opcode>(code[ip++]);

		if (opcode_handlers.count(opcode))
		{
			try
			{
				opcode_handlers[opcode]();
			}
			catch (const std::exception &e)
			{
				status = VMStatus::ERROR;
				std::cerr << "VM Error: " << e.what() << std::endl;
				return status;
			}
		}
		else
		{
			status = VMStatus::EXPLODED;
			throw std::runtime_error("Unknown opcode");
		}

		return VMStatus::OK;
	}

	void VM::init_handlers()
	{

		opcode_handlers[PUSH] = [this]()
		{
			int raw_value = code[ip++];
			push(Value(raw_value));
		};

		opcode_handlers[PUSH_NIL] = [this]()
		{
			push(Value());
		};

		opcode_handlers[ADD] = [this]()
		{
			Value b = pop<Value>();
			Value a = pop<Value>();

			if (a.is_nil() || b.is_nil())
			{
				throw std::runtime_error("Type mismatch: cannot add NIL values");
			}

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

			if (a.is_nil() || b.is_nil())
			{
				throw std::runtime_error("Type mismatch: cannot subtract NIL values");
			}

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

			if (a.is_nil() || b.is_nil())
			{
				throw std::runtime_error("Type mismatch: cannot multiply NIL values");
			}

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

			if (a.is_nil() || b.is_nil())
			{
				throw std::runtime_error("Type mismatch: cannot divide NIL values");
			}

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
			case NIL:
				std::cout << "NIL" << std::endl;
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

		status = VMStatus::OK;
	}

	// friend function for printing vm state
	std::ostream &operator<<(std::ostream &os, const PiELo_VM::VM &vm)
	{
		os << "VM stack: [";
		for (const auto &value : vm.stack)
		{
			if (value.type == PiELo_VM::INT)
			{
				os << value.get_int() << " ";
			}
			else if (value.type == PiELo_VM::BOOL)
			{
				os << (value.get_bool() ? "true" : "false") << " ";
			}
			else if (value.type == PiELo_VM::FLOAT)
			{
				os << value.get_float() << " ";
			}
			else
			{
				os << "Unknown type ";
			}
		}
		os << "]";
		return os;
	}

	// Ensure the namespace is closed here
} // end namespace
