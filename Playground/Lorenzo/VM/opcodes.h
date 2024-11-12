#ifndef OPCODES_H // include guards
#define OPCODES_H

#include <vector>
#include <unordered_map>
#include <functional>
#include "types.h"

namespace PiELo_VM
{

	enum Opcode
	{
		PUSH,
		PUSH_NIL,
		POP,
		ADD,
		SUB,
		MUL,
		DIV,
		PRINT,
		HALT,
		JMP,
		JMP_IF_ZERO
	};

	enum class VMStatus
	{
		OK,
		DONE,
		ERROR,
		EXPLODED
	};

	// alias for opcode handler function
	using opcode_handler = std::function<void()>;

	class VM
	{
	public:
		VM();
		void run(const std::vector<int> &code);
		friend std::ostream &operator<<(std::ostream &os, const VM &vm); // good for debugging

		VMStatus step();
		VMStatus get_status() const { return status; }

		friend std::ostream &operator<<(std::ostream &os, const VM &vm);

	private:
		std::vector<Value> stack;
		std::unordered_map<Opcode, opcode_handler> opcode_handlers; // opcode handler map
		std::vector<int> code;										// program code
		size_t ip;													// instruction pointer
		VMStatus status;

		void init_handlers();

		template <typename T>
		void push(T value)
		{
			stack.push_back(Value(value));
		}

		template <typename T>
		T pop()
		{
			if (stack.empty())
			{
				throw std::runtime_error("Stack underflow");
			}
			T value = stack.back();
			stack.pop_back();
			return value;
		}
	};

} // end namespace

#endif // OPCODES_H
