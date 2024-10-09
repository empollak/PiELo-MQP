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

	// alias for opcode handler function
	using opcode_handler = std::function<void()>;

	class VM
	{
	public:
		VM();
		void run(const std::vector<int> &code);
		friend std::ostream &operator<<(std::ostream &os, const VM &vm); // good for debugging

	private:
		std::vector<Value> stack;
		std::unordered_map<Opcode, opcode_handler> opcode_handlers; // opcode handler map
		std::vector<int> code;										// program code
		size_t ip;													// instruction pointer

		void init_handlers();

		template <typename T>
		void push(T value);

		template <typename T>
		T pop();
	};

} // end namespace

#endif // OPCODES_H
