#include "CLexer.h"
#include <iostream>
#include <limits.h> 

CLexer::CLexer(std::istream& input, std::ofstream& output)
	: m_input(input)
	, m_output(output)
{
}
bool CLexer::FindStringInVector(std::vector<std::string> vector, std::string string)
{
	return std::find(vector.begin(), vector.end(), string) != vector.end();
}

bool CLexer::IsOperator(std::string string)
{
	return FindStringInVector(m_keywords, string);
}

bool CLexer::IsArray(std::string string)
{
	return FindStringInVector(m_array, string);
}

bool CLexer::IsComment(std::string string)
{
	return FindStringInVector(m_comments, string);
}

bool CLexer::IsArithmeticalOperator(std::string string)
{
	return FindStringInVector(m_arithmeticalOperators, string);
}

bool CLexer::IsComparisonOperator(std::string string)
{
	return FindStringInVector(m_comparisonOperators, string);
}

bool CLexer::IsLogicOperator(std::string string)
{
	return FindStringInVector(m_logicOperators, string);
}

bool CLexer::IsBracket(std::string string)
{
	return FindStringInVector(m_brackets, string);
}

bool CLexer::IsSeparator(std::string string)
{
	return FindStringInVector(m_separators, string);
}

bool CLexer::IsBinary(std::string string)
{
	return FindStringInVector(m_binary, string);
}

bool CLexer::IsOctal(std::string string)
{
	return FindStringInVector(m_octal, string);
}

bool CLexer::IsDecimal(std::string string)
{
	return FindStringInVector(m_decimal, string);
}

bool CLexer::IsHexadecimal(std::string string)
{
	return FindStringInVector(m_hexadecimal, string);
}

bool CLexer::IsLetter(std::string string)
{
	return FindStringInVector(m_letters, string);
}

TokenType CLexer::GetTokenType(Token token)
{
	std::string str;
	if (token.type == TokenType::EoF)
		return TokenType::EoF;
	if (IsOperator(token.value))
		return TokenType::OPERATOR;
	if (IsArray(token.value))
		return TokenType::ARRAY;
	if (token.value[0] == '"' && token.value[token.value.length() - 1] == '"')
		return TokenType::STRING;
	if (token.value[0] == '\'' && token.value[token.value.length() - 1] == '\'')
	{
		if (token.value.length() == 3)
			return TokenType::CHAR;
	}
	if (IsArithmeticalOperator(token.value))
		return TokenType::ARITHMETIC_OPERATOR;
	if (IsComparisonOperator(token.value))
		return TokenType::COMPARISON_OPERATOR;
	if (IsLogicOperator(token.value))
		return TokenType::LOGIC_OPERATOR;
	if (IsBracket(token.value))
		return TokenType::BRACKET;
	if (IsSeparator(token.value))
		return TokenType::SEPARATOR;
	for (int i = 0; i < token.value.length(); i++)
	{
		str = token.value[i];
		if (!(IsLetter(str) || IsDecimal(str) || str == "_" || str == "."))
			break;
		str = token.value[0];
		if (IsLetter(str) && i + 1 == token.value.length())
			return TokenType::IDENTIFIER;
	}

	if (token.value.length() > 2)
	{
		std::string base = token.value.substr(0, 2);
		std::string digits;
		
		if (base == "0b")
		{
			digits = token.value.substr(2, -1);
			bool isDigits = true;
			for (int i = 0; i < digits.length(); i++)
			{
				str = digits[i];
				if (!IsBinary(str))
					isDigits = false;
			}
			if (isDigits)
				return TokenType::BINARY;
		}
		else if(base == "0o")
		{
			digits = token.value.substr(2, -1);
			bool isDigits = true;
			for (int i = 0; i < digits.length(); i++)
			{
				str = digits[i];
				if (!IsOctal(str))
					isDigits = false;
			}
			if (isDigits)
				return TokenType::OCTAL;
		}
		else if (base == "0x")
		{
			digits = token.value.substr(2, -1);
			bool isDigits = true;
			for (int i = 0; i < digits.length(); i++)
			{
				str = digits[i];
				if (!IsHexadecimal(str))
					isDigits = false;
			}
			if (isDigits)
				return TokenType::HEXADECIMAL;
		}
	}

	bool wereDot = false;
	bool wereE = false;
	int E = 0;
	for (int i = 0; i < token.value.length(); i++)
	{
		if (wereE)
			E++;
		str = token.value[i];
		if (IsDecimal(str))
			continue;
		else if (str == ".")
			if (wereDot == false && wereE == false)
				wereDot = true;
			else
				return TokenType::ERROR;
		else if (str == "E")
			if (wereE)
				return TokenType::ERROR;
			else
				wereE = true;
		else if ((str == "-" || str == "+") && i > 1)
		{
			if (!token.value[i - 1] == 'E')
				return TokenType::ERROR;
		}
		else
			return TokenType::ERROR;
	}

	if (wereE && E < 2)
		return TokenType::ERROR;
	if (wereDot || wereE)
		return TokenType::FLOAT;
	else
		return TokenType::INTEGER;

	return TokenType::ERROR;
}

void CLexer::PrintTokens()
{
	for (int i = 0; i < m_tokens.size(); i++)
	{
		m_output << GetTokenName(m_tokens[i]) << " " << m_tokens[i].value << " (" << m_tokens[i].line << ":" << m_tokens[i].position << ")" << std::endl;
	}
}

void CLexer::AddToken(Token token)
{
	if (token.value != "")
	{
		token.type = GetTokenType(token);
		if (token.type == TokenType::IDENTIFIER && token.value.length() > 64)
			token.type = TokenType::ERROR;
		if (token.type == TokenType::INTEGER)
		{
			if (token.value.length() > 10)
				token.type = TokenType::ERROR;
			else if (std::stoi(token.value) > INT_MAX)
				token.type = TokenType::ERROR;
		}
		m_tokens.push_back(token);
	}
}

void CLexer::Run()
{
	int lineNumber = 0;
	int i = 0;
	std::string line;
	bool multiStringComment = false;
	bool string = false;
	while (getline(m_input, line))
	{
		lineNumber++;
		Token token;
		std::string ch;
		bool identifier = true;
		for (i = 0; i < line.size(); ++i)
		{
			if (token.value == "")
			{
				token.line = lineNumber;
				token.position = i + 1;
			}

			if (!multiStringComment && !string) {
				if (line[i] == '(' || line[i] == ')') {
					AddToken(token);
					token.value = line[i];
					token.line = lineNumber;
					token.position = i + 1;
					AddToken(token);
					token.value = "";
					continue;
				}
			}

			if (i > 0 && !string && !multiStringComment)
			{
				if (line[i - 1] == '/' and line[i] == '/')
				{
					token.value == "";
					break;
				}
			}

			if (i + 1 == line.length() && string && (line[i] != '"' || line[i] != '\'') && !multiStringComment)
			{
				string = false;
				token.value += line[i];
				AddToken(token);
				token.value = "";
				token.position = i + 1;
				continue;
			}

			if (multiStringComment)
			{
				if (i > 0)
					if (line[i - 1] == '*' and line[i] == '/')
					{
						multiStringComment = false;
					}
				if (i + 1 == line.size() && !multiStringComment)
					AddToken(token);
				continue;
			}

			if (i > 0 && !string)
			{
				if (line[i - 1] == '/' and line[i] == '*')
				{
					multiStringComment = true;
					continue;
				}
			}

			if ((line[i] == ' ' || line[i] == '	') && !string && !multiStringComment)
			{
				AddToken(token);
				token.position = i + 1;
				token.value = "";
				if (i + 1 == line.size() && !multiStringComment)
					AddToken(token);
				continue;
			}

			if (string && !multiStringComment)
			{
				if (line[i] != '"' && line[i] != '\'')
				{
					token.value += line[i];
				}
				else
				{
					token.value += line[i];
					AddToken(token);
					token.position = i + 1;
					string = false;
					token.value = "";
				}
				if (i + 1 == line.size() && !multiStringComment)
					AddToken(token);
				continue;
			}

			if ((line[i] == '"' || line[i] == '\'') && !multiStringComment)
			{
				string = true;
				AddToken(token);
				token.position = i + 1;
				token.value = line[i];
				if (i + 1 == line.size() && !multiStringComment)
					AddToken(token);
				continue;
			}

			if (!multiStringComment && !string) { 
				if (i > 0) {
					if (line[i] == '=' && line[i + 1] != '=' && line[i - 1] != '=') {
						AddToken(token);
						token.position = i + 1;
						token.value = line[i];
						AddToken(token);
						token.value = "";
						continue;
					}
					if (line[i] == '=' && line[i + 1] != '=' && line[i - 1] == '=') {

						token.position = i + 1;
						token.value += line[i];
						AddToken(token);
						token.value = "";
						continue;
					}
				}
				else {
					if (line[i] == '=' && line[i + 1] != '=') {
						AddToken(token);
						token.position = i + 1;
						token.value = line[i];
						AddToken(token);
						token.value = "";
						continue;
					}
					if (line[i] == '=' && line[i + 1] != '=') {

						token.position = i + 1;
						token.value += line[i];
						AddToken(token);
						token.value = "";
						continue;
					}
				}
			}

			if (!multiStringComment && !string) 
			{
				if (line[i] == '}' || line[i] == '{' || line[i] == '[' || line[i] == ']' || line[i] == ',' || line[i] == ';' || line[i] == '+')
				{
					AddToken(token);
					token.position = i + 1;
					token.value = line[i];
					AddToken(token);
					token.value = "";
					continue;
				}
			}

			ch = line[i];

			if ((ch == "-" || ch == "+") && token.value.length() > 0 && identifier == true && !string && !multiStringComment && token.value != "E")
			{
				if (token.value[token.value.length() - 1] == 'E')
				{
					token.value += line[i];
					if (i + 1 == line.size() && !multiStringComment)
						AddToken(token);
					continue;
				}
			}

			if (IsLetter(ch) || IsDecimal(ch) || ch == "." || ch == "_" )
			{
				if (identifier)
				{
					token.value += line[i];
				}
				else
				{
					AddToken(token);
					token.position = i + 1;
					(line[i] == ' ' || line[i] == '	') ? token.value = "" : token.value = line[i];
					identifier = true;
				}
			}
			else
			{
				if (identifier)
				{
					AddToken(token);
					token.position = i + 1;
					if (line[i] == ' ' || line[i] == '	')
						token.value = "";
					else
						token.value = line[i];
					identifier = false;
				}
				else
				{
					if (line[i] != ' ') 
						token.value += line[i]; 
					else
					{
						AddToken(token);
						token.position = i + 1;
						token.value = "";
					}
				}
			}

			if (i + 1 == line.size() && !multiStringComment)
				AddToken(token);
		}
	}

	Token token;
	token.value = "EoF";
	token.type = TokenType::EoF;
	token.line = lineNumber;
	token.position = i;
	AddToken(token);

	PrintTokens();
}


std::string CLexer::GetTokenName(Token token)
{
	switch (token.type)
	{
	case TokenType::IDENTIFIER:
		return "IDENTIFIER";
	case TokenType::INTEGER:
		return "INTEGER";
	case TokenType::FLOAT:
		return "FLOAT";
	case TokenType::BINARY:
		return "BINARY";
	case TokenType::OCTAL:
		return "OCTAL";
	case TokenType::HEXADECIMAL:
		return "HEXADECIMAL";
	case TokenType::CHAR:
		return "CHAR";
	case TokenType::STRING:
		return "STRING";
	case TokenType::ARRAY:
		return "ARRAY";
	case TokenType::OPERATOR:
		return "OPERATOR";
	case TokenType::ARITHMETIC_OPERATOR:
		return "ARITHMETIC_OPERATOR";
	case TokenType::COMPARISON_OPERATOR:
		return "COMPARISON_OPERATOR";
	case TokenType::LOGIC_OPERATOR:
		return "LOGIC_OPERATOR";
	case TokenType::BRACKET:
		return "BRACKET";
	case TokenType::SEPARATOR:
		return "SEPARATOR";
	case TokenType::COMMENT:
		return "COMMENT";
	case TokenType::ERROR:
		return "ERROR";
	case TokenType::EoF:
		return "END_OFF_FILE";
	default:
		return "unknown type";
	}
}
