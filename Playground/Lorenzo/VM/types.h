#ifndef TYPES_H
#define TYPES_H

enum Type
{
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
	};

	Value(int value) : type(INT), int_value(value) {}

	Value(bool value) : type(BOOL), bool_value(value) {}

	Value(float value) : type(FLOAT), float_value(value) {}

	int get_int() const
	{
		if (type != INT)
			throw std::runtime_error("Type mismatch: INT expected");
		return int_value;
	}

	bool get_bool() const
	{
		if (type != BOOL)
			throw std::runtime_error("Type mismatch: BOOL  expected");
		return bool_value;
	}

	float get_float() const
	{
		if (type != FLOAT)
			throw std::runtime_error("Type mismatch: FLOAT  expected");
		return float_value;
	}
};

#endif
