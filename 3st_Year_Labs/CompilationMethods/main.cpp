#include "lexer.h"
#include <unordered_map>
#include <iostream>
#include <string>
#include <cctype>

//===----------------------------------------------------------------------===//
// Finite Automaton Lexer Implementation
//===----------------------------------------------------------------------===//

enum State {
    S,      // Start state
    NUM,    // Number
    LETTER, // Identifier
    EQ,     // = or ==
    NE,     // !=
    LTGT,   // < or > or <= or >=
    DONE    // Final state
};

static int lineNumber = 1;
static int columnNumber = 1;

static const std::unordered_map<std::string, Token> keywords = {
        {"int", tok_int},
        {"double", tok_double},
        {"bool", tok_bool},
        {"true", tok_true},
        {"false", tok_false},
        {"if", tok_if},
        {"else", tok_else},
        {"while", tok_while},
        {"print", tok_print},
        {"endl", tok_endl},
        {"input", tok_input}
};

void reportError(const std::string& message, int line, int column) {
    std::cerr << "Ошибка (строка " << line << ", позиция " << column << "): " << message << std::endl;
}

static int gettok() {
    static int LastChar = ' ';
    static State currentState = S;
    static std::string currentToken;

    while (true) {
        while (isspace(LastChar)) {
            if (LastChar == '\n') {
                lineNumber++;
                columnNumber = 1;
            } else {
                columnNumber++;
            }
            LastChar = InputStream->get();
        }

        switch (currentState) {
            case S: {
                if (LastChar == EOF) return tok_eof;
                if (LastChar == '#') {
                    do {
                        LastChar = InputStream->get();
                        columnNumber++;
                    } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
                    if (LastChar != EOF) continue;
                    return tok_eof;
                }

                if (isdigit(LastChar)) {
                    currentState = NUM;
                    currentToken = LastChar;
                    LastChar = InputStream->get();
                    columnNumber++;
                    continue;
                }
                else if (isalpha(LastChar) || LastChar == '_') {
                    currentState = LETTER;
                    currentToken = LastChar;
                    LastChar = InputStream->get();
                    columnNumber++;
                    continue;
                }
                else if (LastChar == '\"') {
                    StringVal.clear();
                    LastChar = InputStream->get();
                    columnNumber++;
                    while (LastChar != '\"' && LastChar != EOF) {
                        StringVal += LastChar;
                        LastChar = InputStream->get();
                        columnNumber++;
                    }
                    if (LastChar == EOF) {
                        reportError("незавершенная строковая константа", lineNumber, columnNumber);
                        return tok_eof;
                    }
                    LastChar = InputStream->get();
                    columnNumber++;
                    return tok_string;
                }
                else if (LastChar == '=') {
                    currentState = EQ;
                    LastChar = InputStream->get();
                    columnNumber++;
                    continue;
                }
                else if (LastChar == '!') {
                    currentState = NE;
                    LastChar = InputStream->get();
                    columnNumber++;
                    continue;
                }
                else if (LastChar == '<' || LastChar == '>') {
                    currentState = LTGT;
                    currentToken = LastChar;
                    LastChar = InputStream->get();
                    columnNumber++;
                    continue;
                }
                else {
                    int thisChar = LastChar;
                    LastChar = InputStream->get();
                    columnNumber++;
                    switch (thisChar) {
                        case ';': return tok_semi;
                        case '{': return tok_lbrace;
                        case '}': return tok_rbrace;
                        case '[': return tok_lsquare;
                        case ']': return tok_rsquare;
                        case ',': return tok_comma;
                        case '+': return tok_plus;
                        case '-': return tok_minus;
                        case '*': return tok_multiply;
                        case '/': return tok_divide;
                        case '(': return tok_lparen;
                        case ')': return tok_rparen;
                        case '&':
                            if (LastChar == '&') {
                                LastChar = InputStream->get();
                                columnNumber++;
                                return tok_and;
                            }
                            return '&';
                        case '|':
                            if (LastChar == '|') {
                                LastChar = InputStream->get();
                                columnNumber++;
                                return tok_or;
                            }
                            return '|';
                        default:
                            return thisChar;
                    }
                }
                break;
            }

            case NUM: {
                while (isdigit(LastChar) || LastChar == '.') {
                    currentToken += LastChar;
                    LastChar = InputStream->get();
                    columnNumber++;
                }

                if (isalpha(LastChar)) {
                    int errorColumn = columnNumber;
                    int errorLine = lineNumber;
                    while (isalnum(LastChar) || LastChar == '_') {
                        currentToken += LastChar;
                        LastChar = InputStream->get();
                        columnNumber++;
                    }
                    reportError("недопустимый числовой литерал " + currentToken  ,errorLine, errorColumn);
                    currentState = S;
                    currentToken.clear();
                    continue;
                }

                NumVal = strtod(currentToken.c_str(), nullptr);
                currentState = S;
                currentToken.clear();
                return tok_number;
            }

            case LETTER: {
                while (isalnum(LastChar) || LastChar == '_') {
                    currentToken += LastChar;
                    LastChar = InputStream->get();
                    columnNumber++;
                }

                auto it = keywords.find(currentToken);
                if (it != keywords.end()) {
                    if (currentToken == "true") BoolVal = true;
                    if (currentToken == "false") BoolVal = false;
                    Token tok = it->second;
                    currentState = S;
                    currentToken.clear();
                    return tok;
                }

                IdentifierStr = currentToken;
                currentState = S;
                currentToken.clear();
                return tok_identifier;
            }

            case EQ: {
                if (LastChar == '=') {
                    LastChar = InputStream->get();
                    columnNumber++;
                    currentState = S;
                    return tok_eq;
                }
                currentState = S;
                return tok_assign;
            }

            case NE: {
                if (LastChar == '=') {
                    LastChar = InputStream->get();
                    columnNumber++;
                    currentState = S;
                    return tok_ne;
                }
                currentState = S;
                return '!';
            }

            case LTGT: {
                if (LastChar == '=') {
                    LastChar = InputStream->get();
                    columnNumber++;
                    currentState = S;
                    return currentToken[0] == '<' ? tok_le : tok_ge;
                }
                currentState = S;
                return currentToken[0] == '<' ? tok_lt : tok_gt;
            }

            case DONE:
                currentState = S;
                break;
        }
    }
}

int main() {
    std::cout << "Введите код:\n";

    while (true) {
        std::cout << "> ";
        int tok = gettok();

        if (tok == tok_eof) {
            std::cout << "Конец ввода.\n";
            break;
        }
        // Выводим тип токена и его значение (если есть)
        switch (tok) {
            case tok_number:
                std::cout << "Число: " << NumVal << "\n";
                break;
            case tok_identifier:
                std::cout << "Идентификатор: " << IdentifierStr << "\n";
                break;
            case tok_string:
                std::cout << "Строка: \"" << StringVal << "\"\n";
                break;
            case tok_true:
            case tok_false:
                std::cout << "Булево значение: " << (BoolVal ? "true" : "false") << "\n";
                break;
            case tok_int:
                std::cout << "Ключевое слово: int\n";
                break;
            case tok_double:
                std::cout << "Ключевое слово: double\n";
                break;
            case tok_bool:
                std::cout << "Ключевое слово: bool\n";
                break;
            case tok_if:
                std::cout << "Ключевое слово: if\n";
                break;
            case tok_else:
                std::cout << "Ключевое слово: else\n";
                break;
            case tok_while:
                std::cout << "Ключевое слово: while\n";
                break;
            case tok_print:
                std::cout << "Ключевое слово: print\n";
                break;
            case tok_input:
                std::cout << "Ключевое слово: input\n";
                break;
            case tok_eq:
                std::cout << "Оператор: ==\n";
                break;
            case tok_ne:
                std::cout << "Оператор: !=\n";
                break;
            case tok_lt:
                std::cout << "Оператор: <\n";
                break;
            case tok_le:
                std::cout << "Оператор: <=\n";
                break;
            case tok_gt:
                std::cout << "Оператор: >\n";
                break;
            case tok_ge:
                std::cout << "Оператор: >=\n";
                break;
            case tok_and:
                std::cout << "Оператор: &&\n";
                break;
            case tok_or:
                std::cout << "Оператор: ||\n";
                break;
            case tok_assign:
                std::cout << "Оператор: =\n";
                break;
            case tok_plus:
                std::cout << "Оператор: +\n";
                break;
            case tok_minus:
                std::cout << "Оператор: -\n";
                break;
            case tok_multiply:
                std::cout << "Оператор: *\n";
                break;
            case tok_divide:
                std::cout << "Оператор: /\n";
                break;
            case tok_lparen:
                std::cout << "Символ: (\n";
                break;
            case tok_rparen:
                std::cout << "Символ: )\n";
                break;
            case tok_lbrace:
                std::cout << "Символ: {\n";
                break;
            case tok_rbrace:
                std::cout << "Символ: }\n";
                break;
            case tok_lsquare:
                std::cout << "Символ: [\n";
                break;
            case tok_rsquare:
                std::cout << "Символ: ]\n";
                break;
            case tok_comma:
                std::cout << "Символ: ,\n";
                break;
            case tok_semi:
                std::cout << "Символ: ;\n";
                break;
            case tok_endl:
                std::cout << "Конец строки (endl)\n";
                break;
            default:
                if (tok > 0 && isprint(tok)) {
                    std::cout << "Неизвестный символ: '" << static_cast<char>(tok) << "'\n";
                } else {
                    std::cout << "Неизвестный токен (код: " << tok << ")\n";
                }
                break;
        }
    }

    return 0;
}