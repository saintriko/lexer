# lexer
Лексический анализатор ПС-31
Выполнил Семеев Максим
Вход: код программы
input.txt
Выход: список токенов. 
output.txt
Список токенов:
ERROR: неизвестный токен
END_OFF_FILE: конец файла
ID: буквы, “_”, “.”, цифры. Не может начинаться с цифры 
INTEGER: целое число.
FLOAT: числа с плавающей длинной.
CHAR:  ’a’
STRING: “abc”
ARRAY: "[ ]"
BINARY: "0", "1"
OCTAL: "0", "1", "2", "3", "4", "5", "6", "7"
HEXADECIMAL: "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F"
OPERATORS: "if", "else", "while", "for", "read", "write", "return", "int", "float", "void", "main", "string", "char"
ARITHMETIC OPERATORS: "+", "-", "*", "/", "="
COMPARISONS OPERATOR: "==", "!=", ">", "<", ">=", "<="
LOGIC OPERATOR: "&&", "||", "!"
BRACKET: "{", "}", "(", ")"
COMMENT: "//", "/*", "*/"
SEPARATOR: ",", ";", ":"
