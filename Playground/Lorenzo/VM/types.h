#ifndef TYPES_H
#define TYPES_H

#include <stdexcept>

namespace PiELo_VM
{

	enum Type
	{
		NIL,
		INT,
		BOOL,
		FLOAT
	};

	struct Value
	{
		Type type;
		union
		{
			int int_value;
			bool bool_value;
			float float_value;
		} value_union;

		Value() : type(NIL) {}

		Value(int value) : type(INT), value_union{.int_value = value} {}
		Value(bool value) : type(BOOL), value_union{.bool_value = value} {}
		Value(float value) : type(FLOAT), value_union{.float_value = value} {}

		int get_int() const
		{
			if (type != INT)
				throw std::runtime_error("Type mismatch: INT expected");
			return value_union.int_value;
		}

		bool get_bool() const
		{
			if (type != BOOL)
				throw std::runtime_error("Type mismatch: BOOL  expected");
			return value_union.bool_value;
		}

		float get_float() const
		{
			if (type != FLOAT)
				throw std::runtime_error("Type mismatch: FLOAT  expected");
			return value_union.float_value;
		}

		bool is_nil() const
		{
			return type == NIL;
		}
	};

} // end namespace

#endif
