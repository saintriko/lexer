#pragma once
#include <string>

enum class TokenType
{
	IDENTIFIER,
	INTEGER,
	FLOAT,
	BINARY,
	OCTAL,
	HEXADECIMAL,
	CHAR,
	STRING,
	ARRAY,
	OPERATOR,
	ARITHMETIC_OPERATOR,
	COMPARISON_OPERATOR,
	LOGIC_OPERATOR,
	BRACKET,
	SEPARATOR,
	COMMENT,
	ERROR,
	EoF
};

struct Token
{
	TokenType type;
	std::string value;
	size_t line = 0;
	size_t position = 0;
};
