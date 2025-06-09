#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <map>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <limits>

enum Token {
    tok_eof = -1,
    tok_number = -2,
    tok_identifier = -3,
    tok_int_kw = -4,       //  int
    tok_double_kw = -5,    //  "double"
    tok_bool_kw = -6,      // "bool"
    tok_true_val = -7,     //  "true"
    tok_false_val = -8,    // "false"
    tok_eq_cmp = -9,       // ==
    tok_ne = -10,          // !=
    tok_lt = -11,          // <
    tok_le = -12,          // <=
    tok_gt = -13,          // >
    tok_ge = -14,          // >=
    tok_and_logical = -15, // &&
    tok_or_logical = -16,  // ||
    tok_if_kw = -17,       // "if"
    tok_else_kw = -18,     // "else"
    tok_while_kw = -19,    // "while"
    tok_print_kw = -20,    // "print"
    tok_string_lit = -22,  // "string literal"
    tok_input_kw = -23,    // ""inpt"
    tok_comment_lex = -24,

    tok_plus = '+',
    tok_minus = '-',
    tok_multiply = '*',
    tok_divide = '/',
    tok_lparen = '(',
    tok_rparen = ')',
    tok_semi = ';',
    tok_assign = '=',
    tok_lbrace = '{',
    tok_rbrace = '}',
    tok_lsquare = '[',
    tok_rsquare = ']',
    tok_comma = ','
    // tok_lexer_error = -25
};

static int LastChar = ' ';
static std::string IdentifierStr;
static double NumVal;
static std::string StringVal;
static bool BoolVal;
static std::istream* InputStream = &std::cin;

enum LexerState { S_LEX, NUM_LEX, LETTER_LEX, EQ_LEX, NE_LEX, LTGT_LEX, STRING_LEX_INTERN, SLASH_LEX, AND_LEX, OR_LEX };
static LexerState currentLexerState = S_LEX;

static int lineNumber = 1;
static int columnNumber = 1;

static const std::unordered_map<std::string, Token> keywords = {
        {"In", tok_int_kw}, {"int", tok_int_kw},
        {"doub", tok_double_kw}, {"double", tok_double_kw},
        {"bol", tok_bool_kw}, {"bool", tok_bool_kw},
        {"true", tok_true_val}, {"tr", tok_true_val},
        {"false", tok_false_val}, {"Fls", tok_false_val},
        {"if", tok_if_kw}, {"ifs", tok_if_kw},
        {"else", tok_else_kw}, {"Els", tok_else_kw},
        {"while", tok_while_kw}, {"whil", tok_while_kw},
        {"print", tok_print_kw}, {"prnt", tok_print_kw},
        {"input", tok_input_kw}, {"inpt", tok_input_kw}
};

static void reportLexerError(const std::string& message, int line, int col) {
    std::ostringstream error_message_stream;
    error_message_stream << "Лексическая ошибка (строка " << line << ", позиция " << col << "): " << message;
    std::cerr << error_message_stream.str() << std::endl;
    throw std::runtime_error(error_message_stream.str());
}
static void reportParserError(const std::string& message, int line, int col) {
    std::ostringstream error_message_stream;
    error_message_stream << "Синтаксическая/Семантическая ошибка (строка " << lineNumber << ", позиция " << columnNumber << "): " << message;
    std::cerr << error_message_stream.str() << std::endl;
}

std::string char_to_debug_str_lex(int c) {
    if (c == EOF) return "EOF";
    if (isprint(c)) return "'" + std::string(1, (char)c) + "'";
    if (c == '\n') return "'\\n'";
    if (c == '\r') return "'\\r'";
    if (c == '\t') return "'\\t'";
    return "ASCII(" + std::to_string(c) + ")";
}

static int gettok() {
    static std::string currentTokenBuild;
    while (true) {
        while (isspace(LastChar)) {
            if (LastChar == '\n') { lineNumber++; columnNumber = 1; }
            else { columnNumber++; }
            LastChar = InputStream->get();
            if (InputStream->eof()) LastChar = EOF;
        }

        if (LastChar == EOF && currentLexerState == S_LEX) return tok_eof;
        if (!InputStream->good() && !InputStream->eof() && currentLexerState == S_LEX) {
            reportLexerError("Ошибка чтения из входного потока (не EOF)", lineNumber, columnNumber);
            LastChar = EOF; return tok_eof;
        }

        int initialCol = columnNumber;

        switch (currentLexerState) {
            case S_LEX: {
                currentTokenBuild.clear();
                if (LastChar == EOF) return tok_eof;

                if (LastChar == '#') {
                    do {
                        LastChar = InputStream->get();
                        if (InputStream->eof()) { LastChar = EOF; break;}
                        if (LastChar == '\n') { lineNumber++; columnNumber = 1; } else { columnNumber++;}
                    } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
                    if (LastChar != EOF) {
                        LastChar = InputStream->get(); if(InputStream->eof()) LastChar = EOF; columnNumber++;
                    }
                    continue;
                }

                if (isalpha(LastChar) || LastChar == '_') {
                    currentTokenBuild += (char)LastChar;
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    currentLexerState = LETTER_LEX;
                    continue;
                }

                if (isdigit(LastChar)) {
                    currentTokenBuild += (char)LastChar;
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    currentLexerState = NUM_LEX;
                    continue;
                }

                if (LastChar == '\"') {
                    StringVal.clear();
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    currentLexerState = STRING_LEX_INTERN;
                    continue;
                }

                if (LastChar == '=') { currentLexerState = EQ_LEX; LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF; continue;}
                if (LastChar == '!') { currentTokenBuild += (char)LastChar; currentLexerState = NE_LEX; LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF; continue;}
                if (LastChar == '<' || LastChar == '>') { currentTokenBuild += (char)LastChar; currentLexerState = LTGT_LEX; LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF; continue;}
                if (LastChar == '&') { currentLexerState = AND_LEX; LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF; continue;}
                if (LastChar == '|') { currentLexerState = OR_LEX; LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF; continue;}

                if (strchr("+-*/();{}[] ,", LastChar)) {
                    int thisCharVal = LastChar;
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    return thisCharVal;
                }
                {
                    int unknownChar = LastChar;
                    reportLexerError("неизвестный символ: " + char_to_debug_str_lex(unknownChar), lineNumber, initialCol);
                    return unknownChar;
                }
            }

            case NUM_LEX: {
                bool contains_dot = false;
                for(char c_val : currentTokenBuild) if(c_val == '.') contains_dot = true;
                while (LastChar != EOF && (isdigit(LastChar) || (LastChar == '.' && !contains_dot))) {
                    if (LastChar == '.') contains_dot = true;
                    currentTokenBuild += (char)LastChar;
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                }
                if (LastChar != EOF && (isalpha(LastChar) || LastChar == '_')) {
                    std::string invalid_suffix_part;
                    while (LastChar != EOF && (isalnum(LastChar) || LastChar == '_')) {
                        invalid_suffix_part += (char)LastChar;
                        LastChar = InputStream->get(); if (InputStream->eof()) LastChar = EOF;
                        if (LastChar != EOF) { columnNumber++; }
                    }
                    currentLexerState = S_LEX; currentTokenBuild.clear();
                    reportLexerError("недопустимый числовой литерал, содержит буквы/подчеркивания после числа: " + currentTokenBuild + invalid_suffix_part, lineNumber, initialCol);
                    continue;
                }

                char* endPtr;
                NumVal = strtod(currentTokenBuild.c_str(), &endPtr);
                if (*endPtr != '\0' && endPtr != currentTokenBuild.c_str() + currentTokenBuild.length()) {
                    reportLexerError("ошибка преобразования числового литерала: " + currentTokenBuild, lineNumber, initialCol);
                }
                currentLexerState = S_LEX;
                return tok_number;
            }

            case LETTER_LEX: {
                while (LastChar != EOF && (isalnum(LastChar) || LastChar == '_')) {
                    currentTokenBuild += (char)LastChar;
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                }
                IdentifierStr = currentTokenBuild;
                currentLexerState = S_LEX;
                auto it = keywords.find(IdentifierStr);
                if (it != keywords.end()) {
                    if (it->second == tok_true_val) BoolVal = true;
                    else if (it->second == tok_false_val) BoolVal = false;
                    return it->second;
                }
                return tok_identifier;
            }

            case STRING_LEX_INTERN: {
                int string_start_col = initialCol;
                while (LastChar != '\"' && LastChar != EOF) {
                    if (LastChar == '\n' || LastChar == '\r') {
                        reportLexerError("незавершенная строковая константа (обнаружен конец строки)", lineNumber, string_start_col);
                        currentLexerState = S_LEX; StringVal.clear();
                        return tok_string_lit;
                    }
                    if (LastChar == '\\') {
                        LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                        if (LastChar == EOF) {
                            reportLexerError("незавершенная escape-последовательность в строке (EOF)", lineNumber, columnNumber-1);
                            break;
                        }
                        switch (LastChar) {
                            case 'n': StringVal += '\n'; break;
                            case 't': StringVal += '\t'; break;
                            case '"': StringVal += '"'; break;
                            case '\\': StringVal += '\\'; break;
                            default: StringVal += '\\'; StringVal += (char)LastChar; break;
                        }
                    } else {
                        StringVal += (char)LastChar;
                    }
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                }
                if (LastChar == EOF) {
                    reportLexerError("незавершенная строковая константа (EOF)", lineNumber, string_start_col);
                    currentLexerState = S_LEX;
                    return tok_eof;
                }
                LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF; // Consume closing quote
                currentLexerState = S_LEX;
                return tok_string_lit;
            }

            case EQ_LEX: {
                currentLexerState = S_LEX;
                if (LastChar == '=') {
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    return tok_eq_cmp;
                }
                return tok_assign;
            }
            case NE_LEX: {
                currentLexerState = S_LEX;
                if (LastChar == '=') {
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    return tok_ne;
                }
                reportLexerError("ожидался '=' после '!' для оператора '!='", lineNumber, initialCol);
                return '!';
            }
            case LTGT_LEX: {
                currentLexerState = S_LEX;
                char firstOpChar = currentTokenBuild[0];
                if (LastChar == '=') {
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    return firstOpChar == '<' ? tok_le : tok_ge;
                }
                return firstOpChar == '<' ? tok_lt : tok_gt;
            }
            case AND_LEX: {
                currentLexerState = S_LEX;
                if (LastChar == '&') {
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    return tok_and_logical;
                }
                reportLexerError("одиночный '&' не поддерживается, ожидался '&&'", lineNumber, initialCol);
                return '&';
            }
            case OR_LEX: {
                currentLexerState = S_LEX;
                if (LastChar == '|') {
                    LastChar = InputStream->get(); columnNumber++; if (InputStream->eof()) LastChar = EOF;
                    return tok_or_logical;
                }
                reportLexerError("одиночный '|' не поддерживается, ожидался '||'", lineNumber, initialCol);
                return '|';
            }
            default: {
                reportLexerError("КРИТИЧЕСКАЯ ОШИБКА - gettok() в неизвестном состоянии КА: " + std::to_string(currentLexerState), lineNumber, columnNumber);
                LastChar = EOF; return tok_eof;
            }
        }
    }
}

enum class UserVarType { UNKNOWN, INTEGER, DOUBLE, BOOLEAN, STRING };
enum class RPNValueType {
    CONSTANT_INT, CONSTANT_DOUBLE, CONSTANT_BOOL, CONSTANT_STRING,VARIABLE_ID,ARRAY_BASE_ID,OPERATION_ID,LABEL_PLACEHOLDER,RAW_LABEL_TARGET
};
enum class RPNOperationId {
    OP_UNARY_MINUS = 1, OP_ADD = 2, OP_SUB = 3, OP_MUL = 4, OP_DIV = 5,
    OP_ASSIGN = 6, OP_GT = 7, OP_LT = 8, OP_GE = 9, OP_LE = 10,
    OP_EQ_CMP = 11, OP_NE = 12, OP_JUMP = 13, OP_JUMP_FALSE = 14,
    OP_ALLOC_MEM_SCALAR = 15, OP_INDEX_ARRAY = 16, OP_READ_INPUT = 17,
    OP_WRITE_OUTPUT = 18, OP_ALLOC_MEM_ARRAY = 19, OP_DEALLOC_MEM = 20,
    OP_UNARY_PLUS = 21, OP_LOGICAL_AND = 22, OP_LOGICAL_OR = 23,
    OP_INDEX_ARRAY_2D = 24, OP_INIT_ARRAY = 25
};

std::string rpnOpIdToString(RPNOperationId op_id) {
    switch (op_id) {
        case RPNOperationId::OP_UNARY_MINUS: return "u-";
        case RPNOperationId::OP_UNARY_PLUS:  return "u+";
        case RPNOperationId::OP_ADD: return "+";
        case RPNOperationId::OP_SUB: return "-";
        case RPNOperationId::OP_MUL: return "*";
        case RPNOperationId::OP_DIV: return "/";
        case RPNOperationId::OP_ASSIGN: return ":=";
        case RPNOperationId::OP_GT: return ">";
        case RPNOperationId::OP_LT: return "<";
        case RPNOperationId::OP_GE: return ">=";
        case RPNOperationId::OP_LE: return "<=";
        case RPNOperationId::OP_EQ_CMP: return "==";
        case RPNOperationId::OP_NE: return "!=";
        case RPNOperationId::OP_LOGICAL_AND: return "&&";
        case RPNOperationId::OP_LOGICAL_OR: return "||";
        case RPNOperationId::OP_JUMP: return "j";
        case RPNOperationId::OP_JUMP_FALSE: return "jf";
        case RPNOperationId::OP_ALLOC_MEM_SCALAR: return "alloc_S";
        case RPNOperationId::OP_ALLOC_MEM_ARRAY: return "alloc_A";
        case RPNOperationId::OP_DEALLOC_MEM: return "dealloc";
        case RPNOperationId::OP_INDEX_ARRAY: return "idx";
        case RPNOperationId::OP_INDEX_ARRAY_2D: return "idx2D";
        case RPNOperationId::OP_INIT_ARRAY: return "init_A";
        case RPNOperationId::OP_READ_INPUT: return "read";
        case RPNOperationId::OP_WRITE_OUTPUT: return "write";
        default: return "UNKNOWN_OP_ID(" + std::to_string(static_cast<int>(op_id)) + ")";
    }
}

struct OPS_Entry {
    RPNValueType rpn_val_type;
    union Def {
        int    ival;
        double dval;
        bool   bval;
        char* sval;
        RPNOperationId op_id;
        Def() { memset(this, 0, sizeof(Def)); }
        ~Def() {}
    } def;
    std::string comment;
    UserVarType var_operand_type;
    OPS_Entry() : rpn_val_type(RPNValueType::OPERATION_ID), comment(""), var_operand_type(UserVarType::UNKNOWN) {
        def.op_id = RPNOperationId::OP_ADD;
    }
    ~OPS_Entry() {
        if (rpn_val_type == RPNValueType::CONSTANT_STRING && def.sval != nullptr) {
            delete[] def.sval;
        }
    }
    OPS_Entry(const OPS_Entry& other) : rpn_val_type(other.rpn_val_type), comment(other.comment), var_operand_type(other.var_operand_type) {
        if (rpn_val_type == RPNValueType::CONSTANT_STRING && other.def.sval) {
            def.sval = new char[strlen(other.def.sval) + 1];
            strcpy(def.sval, other.def.sval);
        } else {
            def = other.def;
        }
    }
    OPS_Entry(OPS_Entry&& other) noexcept : rpn_val_type(other.rpn_val_type), def(other.def), comment(std::move(other.comment)), var_operand_type(other.var_operand_type) {
        if (rpn_val_type == RPNValueType::CONSTANT_STRING) {
            other.def.sval = nullptr;
        }
    }
    OPS_Entry& operator=(const OPS_Entry& other) {
        if (this == &other) return *this;
        if (rpn_val_type == RPNValueType::CONSTANT_STRING && def.sval) {
            delete[] def.sval;
            def.sval = nullptr;
        }

        rpn_val_type = other.rpn_val_type;
        comment = other.comment;
        var_operand_type = other.var_operand_type;

        if (rpn_val_type == RPNValueType::CONSTANT_STRING && other.def.sval) {
            def.sval = new char[strlen(other.def.sval) + 1];
            strcpy(def.sval, other.def.sval);
        } else {
            def = other.def;
        }
        return *this;
    }
    OPS_Entry& operator=(OPS_Entry&& other) noexcept {
        if (this == &other) return *this;

        if (rpn_val_type == RPNValueType::CONSTANT_STRING && def.sval) {
            delete[] def.sval;
            def.sval = nullptr;
        }

        rpn_val_type = other.rpn_val_type;
        def = other.def;
        comment = std::move(other.comment);
        var_operand_type = other.var_operand_type;

        if (rpn_val_type == RPNValueType::CONSTANT_STRING) {
            other.def.sval = nullptr;
        }
        return *this;
    }

    std::string toString() const {
        std::ostringstream oss;
        switch (rpn_val_type) {
            case RPNValueType::CONSTANT_INT:    oss << "CONST_INT:" << def.ival; break;
            case RPNValueType::CONSTANT_DOUBLE: oss << "CONST_DBL:" << def.dval; break;
            case RPNValueType::CONSTANT_BOOL:   oss << "CONST_BOOL:" << (def.bval ? "tr" : "Fls"); break;
            case RPNValueType::CONSTANT_STRING: oss << "CONST_STR:\"" << (def.sval ? def.sval : "") << "\""; break;
            case RPNValueType::VARIABLE_ID:     oss << "VAR_ID:" << def.ival; break;
            case RPNValueType::ARRAY_BASE_ID:   oss << "ARR_ID:" << def.ival; break;
            case RPNValueType::OPERATION_ID:    oss << "OP:" << rpnOpIdToString(def.op_id);
                if (def.op_id == RPNOperationId::OP_ALLOC_MEM_SCALAR || def.op_id == RPNOperationId::OP_ALLOC_MEM_ARRAY) {
                    oss << " (type_hint:" << static_cast<int>(var_operand_type) << ")";
                }
                break;
            case RPNValueType::LABEL_PLACEHOLDER: oss << "LBL_PH:" << def.ival; break;
            case RPNValueType::RAW_LABEL_TARGET:  oss << "LBL_TGT:" << def.ival; break;
            default: oss << "UNKNOWN_RPN_VAL_TYPE";
        }
        if (!comment.empty()) { oss << "\t # " << comment; }
        return oss.str();
    }
};

struct SymbolInfo {
    std::string name;
    UserVarType type;
    bool is_array;
    int array_size;
    int id;
    int declaration_rpn_start_idx;

    SymbolInfo(std::string n="", UserVarType t=UserVarType::UNKNOWN, bool arr=false, int size=0, int i=-1)
            : name(std::move(n)), type(t), is_array(arr), array_size(size), id(i), declaration_rpn_start_idx(-1) {}
};

static int CurTok_parser;
static std::vector<OPS_Entry> rpn_code_parser;
static int current_rpn_idx_parser = 0;
static std::map<std::string, SymbolInfo> symbol_table_parser;
static int next_var_id_parser = 1;
static std::stack<int> label_stack_parser;
void getNextToken_parser() { CurTok_parser = gettok(); }
void addRPN_const_int(int val, const std::string& comment = "") {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::CONSTANT_INT; entry.def.ival = val;
    entry.comment = comment.empty() ? ("Num: " + std::to_string(val)) : comment;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
void addRPN_const_double(double val, const std::string& comment = "") {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::CONSTANT_DOUBLE; entry.def.dval = val;
    entry.comment = comment.empty() ? ("doub: " + std::to_string(val)) : comment;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
void addRPN_const_bool(bool val, const std::string& comment = "") {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::CONSTANT_BOOL; entry.def.bval = val;
    entry.comment = comment.empty() ? (val ? "tr" : "Fls") : comment;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
void addRPN_const_string(const std::string& s_val, const std::string& comment = "") {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::CONSTANT_STRING;
    entry.def.sval = new char[s_val.length() + 1]; strcpy(entry.def.sval, s_val.c_str());
    entry.comment = comment.empty() ? ("strl: \"" + s_val + "\"") : comment;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
void addRPN_var_id(int var_id, const std::string& name_for_comment = "", UserVarType type_hint = UserVarType::UNKNOWN) {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::VARIABLE_ID; entry.def.ival = var_id;
    entry.comment = "VAR " + name_for_comment + "(" + std::to_string(var_id) + ")";
    entry.var_operand_type = type_hint;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
void addRPN_array_base_id(int array_id, const std::string& name_for_comment = "", UserVarType type_hint = UserVarType::UNKNOWN) {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::ARRAY_BASE_ID; entry.def.ival = array_id;
    entry.comment = "ARR " + name_for_comment + "(" + std::to_string(array_id) + ")";
    entry.var_operand_type = type_hint;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
void addRPN_op(RPNOperationId op_id, const std::string& comment_override = "", UserVarType type_hint_for_alloc = UserVarType::UNKNOWN) {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::OPERATION_ID; entry.def.op_id = op_id;
    entry.comment = comment_override.empty() ? rpnOpIdToString(op_id) : comment_override;
    if (op_id == RPNOperationId::OP_ALLOC_MEM_SCALAR || op_id == RPNOperationId::OP_ALLOC_MEM_ARRAY) {
        entry.var_operand_type = type_hint_for_alloc;
    }
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}
int addRPN_label_placeholder(const std::string& comment = "LBL_PH") {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::LABEL_PLACEHOLDER; entry.def.ival = -1;
    entry.comment = comment;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
    return current_rpn_idx_parser - 1;
}
void patchRPN_label(int placeholder_rpn_idx, int target_rpn_address) {
    if (placeholder_rpn_idx >= 0 && static_cast<size_t>(placeholder_rpn_idx) < rpn_code_parser.size()) {
        if (rpn_code_parser[placeholder_rpn_idx].rpn_val_type == RPNValueType::LABEL_PLACEHOLDER) {
            rpn_code_parser[placeholder_rpn_idx].def.ival = target_rpn_address;
            rpn_code_parser[placeholder_rpn_idx].comment += " -> " + std::to_string(target_rpn_address);
        } else {
            std::cerr << "Попытка исправления не-плейсхолдера метки по индексу ОПЗ: " << placeholder_rpn_idx << std::endl;
            throw std::logic_error("Patching non-label placeholder at RPN idx: " + std::to_string(placeholder_rpn_idx));
        }
    } else {
        std::cerr << "Индекс для исправления метки вне диапазона ОПЗ: " << placeholder_rpn_idx << std::endl;
        throw std::out_of_range("RPN label patch index out of range: " + std::to_string(placeholder_rpn_idx));
    }
}
void addRPN_raw_label_target(int target_rpn_addr, const std::string& comment = "") {
    OPS_Entry entry; entry.rpn_val_type = RPNValueType::RAW_LABEL_TARGET; entry.def.ival = target_rpn_addr;
    entry.comment = comment.empty() ? ("Target LBL(" + std::to_string(target_rpn_addr) + ")") : comment;
    rpn_code_parser.push_back(std::move(entry)); current_rpn_idx_parser++;
}

enum class NonTerminal {
    P_NT, A_NT, Z_NT, X_NT, R_NT, M_NT, S_NT, C_NT, T_NT, F_NT, G_NT, H_NT, B_NT,
    I_NT, E_NT, L_NT, S_PRIME_NT, C_PRIME_NT, T_PRIME_NT, F_PRIME_NT, G_PRIME_NT,
    B_ARR_SUFFIX_NT, X_PRIME_NT, R_PRIME_NT, START_SYMBOL, PROGRAM, STMT_LIST,
    STMT, DECL_STMT, ASSIGN_STMT, IF_STMT, ELSE_PART, WHILE_STMT, BLOCK,
    PRINT_STMT, INPUT_STMT, TYPE, IDENT_LIST, ARRAY_DECL_PART,
    EXPRESSION, C_EXPR, T_EXPR, F_EXPR, G_EXPR, H_EXPR, M_DECL_SUFFIX,
    HANDLE_IF_END_MARKER_NT,OPTIONAL_ACTUAL_ELSE_NT,PROG4_ELSE_MARKER_NT,X_ASSIGN_OP_MARKER_NT
};

enum class SemanticActionId {
    ACTION_NONE, ACTION_ADD_IDENT_A, ACTION_ADD_CONST_K,
    ACTION_ADD_OP_PLUS, ACTION_ADD_OP_MINUS, ACTION_ADD_OP_MUL, ACTION_ADD_OP_DIV,
    ACTION_ADD_OP_UNARY_PLUS, ACTION_ADD_OP_UNARY_MINUS, ACTION_ADD_OP_ASSIGN,
    ACTION_ADD_OP_EQ_CMP, ACTION_ADD_OP_NE, ACTION_ADD_OP_LT, ACTION_ADD_OP_LE,
    ACTION_ADD_OP_GT, ACTION_ADD_OP_GE, ACTION_ADD_OP_LOGIC_OR, ACTION_ADD_OP_LOGIC_AND,
    ACTION_OP_WRITE_W, ACTION_OP_READ_R, ACTION_INDEX_ARRAY_I, ACTION_INDEX_ARRAY_I2,
    PROG1_IF_AFTER_COND_GEN_JF, PROG3_IF_AFTER_THEN_PATCH_JF_GEN_JUMP,
    PROG4_IF_AFTER_ELSE_PATCH_JUMP, PROG5_WHILE_BEFORE_COND_PUSH_LABEL,
    PROG_WHILE_AFTER_COND_GEN_JF, PROG6_WHILE_AFTER_BODY_GEN_JUMP_PATCH_JF,
    PROG_SET_DECL_TYPE_INT, PROG_SET_DECL_TYPE_DOUBLE, PROG_SET_DECL_TYPE_BOOL,
    PROG_SET_DECL_TYPE_STRING, PROG10_PROCESS_DECLARED_IDENTIFIER,
    ACTION_SET_ARRAY_SIZE, PROG8_FINALIZE_DECLARATION, ACTION_INIT_ARRAY_ITEMS,
    PROG9_DEALLOCATE_MEM_END_PROGRAM, ACTION_PUSH_RPN_IDX_TO_LABEL_STACK,
};

struct GrammarSymbol {
    bool is_terminal;
    union USymbol {
        Token terminal;
        NonTerminal non_terminal;
        USymbol() : terminal(tok_eof) {}
    } symbol;
    GrammarSymbol(Token t) : is_terminal(true) { symbol.terminal = t; }
    GrammarSymbol(NonTerminal nt) : is_terminal(false) { symbol.non_terminal = nt; }
    GrammarSymbol() : is_terminal(true) { symbol.terminal = tok_eof; }
    std::string toString() const {
        if (is_terminal) return "T:" + std::to_string(static_cast<int>(symbol.terminal));
        return "NT:" + std::to_string(static_cast<int>(symbol.non_terminal));
    }
    bool operator<(const GrammarSymbol& other) const {
        if (is_terminal != other.is_terminal) return is_terminal < other.is_terminal;
        if (is_terminal) return symbol.terminal < other.symbol.terminal;
        return symbol.non_terminal < other.symbol.non_terminal;
    }
};

struct ProductionRule {
    std::vector<GrammarSymbol> rhs_symbols;
    std::vector<SemanticActionId> semantic_actions_for_rhs;
    ProductionRule() = default;
};

using ParsingTable = std::map<NonTerminal, std::map<Token, ProductionRule>>;
static ParsingTable ll1_parsing_table;
static std::stack<GrammarSymbol> parser_stack;
static std::stack<SemanticActionId> generator_action_stack;

static UserVarType current_declaration_type = UserVarType::UNKNOWN;
static std::string last_declared_identifier_name;
static bool last_declaration_is_array = false;
static int last_declaration_array_size = 0;


Token tokenFromString(const std::string& s_token_name) {
    if (s_token_name == "Idenf") return tok_identifier;
    if (s_token_name == "Num") return tok_number;
    if (s_token_name == "StrL" || s_token_name == "strl") return tok_string_lit;
    if (s_token_name == "In" || s_token_name == "int") return tok_int_kw;
    if (s_token_name == "doub"|| s_token_name == "double") return tok_double_kw;
    if (s_token_name == "bol" || s_token_name == "bool") return tok_bool_kw;
    if (s_token_name == "tr" || s_token_name == "true") return tok_true_val;
    if (s_token_name == "Fls"|| s_token_name == "false") return tok_false_val;
    if (s_token_name == "ifs"|| s_token_name == "if") return tok_if_kw;
    if (s_token_name == "Els"|| s_token_name == "else") return tok_else_kw;
    if (s_token_name == "whil"|| s_token_name == "while") return tok_while_kw;
    if (s_token_name == "prnt"|| s_token_name == "print") return tok_print_kw;
    if (s_token_name == "inpt"|| s_token_name == "input") return tok_input_kw;
    if (s_token_name == "eof" || s_token_name == "$") return tok_eof;
    if (s_token_name.length() == 1) {
        char c = s_token_name[0];
        switch(c) {
            case '+': return tok_plus; case '-': return tok_minus; case '*': return tok_multiply;
            case '/': return tok_divide; case '(': return tok_lparen; case ')': return tok_rparen;
            case ';': return tok_semi;   case '=': return tok_assign;   case '{': return tok_lbrace;
            case '}': return tok_rbrace; case '[': return tok_lsquare; case ']': return tok_rsquare;
            case ',': return tok_comma; case '<': return tok_lt; case '>': return tok_gt;
        }
    }
    if (s_token_name == "==") return tok_eq_cmp; if (s_token_name == "!=") return tok_ne;
    if (s_token_name == "<=") return tok_le;   if (s_token_name == ">=") return tok_ge;
    if (s_token_name == "&&") return tok_and_logical; if (s_token_name == "||") return tok_or_logical;
    std::cerr << "tokenFromString: Неизвестное имя терминала: " + s_token_name << std::endl;
    throw std::runtime_error("Unknown terminal string for table population: " + s_token_name);
}

NonTerminal nonTerminalFromString(const std::string& s_nt_name) {
    if (s_nt_name == "P" || s_nt_name == "P_NT") return NonTerminal::P_NT;
    if (s_nt_name == "A" || s_nt_name == "A_NT") return NonTerminal::A_NT;
    if (s_nt_name == "Z" || s_nt_name == "Z_NT") return NonTerminal::Z_NT;
    if (s_nt_name == "X" || s_nt_name == "X_NT") return NonTerminal::X_NT;
    if (s_nt_name == "R" || s_nt_name == "R_NT") return NonTerminal::R_NT;
    if (s_nt_name == "M" || s_nt_name == "M_NT") return NonTerminal::M_NT;
    if (s_nt_name == "S" || s_nt_name == "S_NT") return NonTerminal::S_NT;
    if (s_nt_name == "C" || s_nt_name == "C_NT") return NonTerminal::C_NT;
    if (s_nt_name == "T" || s_nt_name == "T_NT") return NonTerminal::T_NT;
    if (s_nt_name == "F" || s_nt_name == "F_NT") return NonTerminal::F_NT;
    if (s_nt_name == "G" || s_nt_name == "G_NT") return NonTerminal::G_NT;
    if (s_nt_name == "H" || s_nt_name == "H_NT") return NonTerminal::H_NT;
    if (s_nt_name == "B" || s_nt_name == "B_NT") return NonTerminal::B_NT;
    if (s_nt_name == "I" || s_nt_name == "I_NT") return NonTerminal::I_NT;
    if (s_nt_name == "E" || s_nt_name == "E_NT") return NonTerminal::E_NT;
    if (s_nt_name == "L" || s_nt_name == "L_NT") return NonTerminal::L_NT;
    if (s_nt_name == "S_PRIME_NT" || s_nt_name == "S'") return NonTerminal::S_PRIME_NT;
    if (s_nt_name == "C_PRIME_NT" || s_nt_name == "C'") return NonTerminal::C_PRIME_NT;
    if (s_nt_name == "T_PRIME_NT" || s_nt_name == "T'") return NonTerminal::T_PRIME_NT;
    if (s_nt_name == "F_PRIME_NT" || s_nt_name == "F'") return NonTerminal::F_PRIME_NT;
    if (s_nt_name == "G_PRIME_NT" || s_nt_name == "G'") return NonTerminal::G_PRIME_NT;
    if (s_nt_name == "B_ARR_SUFFIX_NT") return NonTerminal::B_ARR_SUFFIX_NT;
    if (s_nt_name == "X_PRIME_NT") return NonTerminal::X_PRIME_NT;
    if (s_nt_name == "R_PRIME_NT") return NonTerminal::R_PRIME_NT;
    if (s_nt_name == "START_SYMBOL") return NonTerminal::START_SYMBOL;
    if (s_nt_name == "Программа" || s_nt_name == "PROGRAM") return NonTerminal::PROGRAM;
    if (s_nt_name == "СписокОператоров" || s_nt_name == "STMT_LIST") return NonTerminal::STMT_LIST;
    if (s_nt_name == "Оператор" || s_nt_name == "STMT") return NonTerminal::STMT;
    if (s_nt_name == "Объявление" || s_nt_name == "DECL_STMT") return NonTerminal::DECL_STMT;
    if (s_nt_name == "Присваивание" || s_nt_name == "ASSIGN_STMT") return NonTerminal::ASSIGN_STMT;
    if (s_nt_name == "Если" || s_nt_name == "IF_STMT") return NonTerminal::IF_STMT;
    if (s_nt_name == "ИначеЧасть" || s_nt_name == "ELSE_PART") return NonTerminal::ELSE_PART;
    if (s_nt_name == "Пока" || s_nt_name == "WHILE_STMT") return NonTerminal::WHILE_STMT;
    if (s_nt_name == "Блок" || s_nt_name == "BLOCK") return NonTerminal::BLOCK;
    if (s_nt_name == "Печать" || s_nt_name == "PRINT_STMT") return NonTerminal::PRINT_STMT;
    if (s_nt_name == "Ввод" || s_nt_name == "INPUT_STMT") return NonTerminal::INPUT_STMT;
    if (s_nt_name == "Тип" || s_nt_name == "TYPE") return NonTerminal::TYPE;
    if (s_nt_name == "СписокИдентификаторов" || s_nt_name == "IDENT_LIST") return NonTerminal::IDENT_LIST;
    if (s_nt_name == "СуффиксСпискаИдентификаторов" || s_nt_name == "M_DECL_SUFFIX") return NonTerminal::M_DECL_SUFFIX;
    if (s_nt_name == "ЧастьОбъявленияМассива" || s_nt_name == "ARRAY_DECL_PART") return NonTerminal::ARRAY_DECL_PART;
    if (s_nt_name == "Выражение" || s_nt_name == "EXPRESSION") return NonTerminal::S_NT; // S_NT is main expression
    if (s_nt_name == "C_Выражение" || s_nt_name == "C_EXPR") return NonTerminal::C_NT;
    if (s_nt_name == "T_Выражение" || s_nt_name == "T_EXPR") return NonTerminal::T_NT;
    if (s_nt_name == "F_Выражение" || s_nt_name == "F_EXPR") return NonTerminal::F_NT;
    if (s_nt_name == "G_Выражение" || s_nt_name == "G_EXPR") return NonTerminal::G_NT;
    if (s_nt_name == "H_Выражение" || s_nt_name == "H_EXPR") return NonTerminal::H_NT;
    if (s_nt_name == "HANDLE_IF_END_MARKER_NT") return NonTerminal::HANDLE_IF_END_MARKER_NT;
    if (s_nt_name == "OPTIONAL_ACTUAL_ELSE_NT") return NonTerminal::OPTIONAL_ACTUAL_ELSE_NT;
    if (s_nt_name == "PROG4_ELSE_MARKER_NT") return NonTerminal::PROG4_ELSE_MARKER_NT;
    if (s_nt_name == "X_ASSIGN_OP_MARKER_NT") return NonTerminal::X_ASSIGN_OP_MARKER_NT;

    std::cerr << "nonTerminalFromString: Неизвестное имя нетерминала: " + s_nt_name << std::endl;
    throw std::runtime_error("Unknown non-terminal string for table population: " + s_nt_name);
}

SemanticActionId semanticActionFromString(const std::string& s_action_cue_full) {
    std::string s_action_cue = s_action_cue_full;
    if (s_action_cue.length() >= 2 && s_action_cue.front() == '[' && s_action_cue.back() == ']') {
        s_action_cue = s_action_cue.substr(1, s_action_cue.length() - 2);
    }

    if (s_action_cue.empty() || s_action_cue == "□" || s_action_cue == "NONE") return SemanticActionId::ACTION_NONE;
    if (s_action_cue == "a") return SemanticActionId::ACTION_ADD_IDENT_A;
    if (s_action_cue == "k") return SemanticActionId::ACTION_ADD_CONST_K;
    if (s_action_cue == "+") return SemanticActionId::ACTION_ADD_OP_PLUS;
    if (s_action_cue == "-") return SemanticActionId::ACTION_ADD_OP_MINUS;
    if (s_action_cue == "*") return SemanticActionId::ACTION_ADD_OP_MUL;
    if (s_action_cue == "/") return SemanticActionId::ACTION_ADD_OP_DIV;
    if (s_action_cue == "u-" || s_action_cue == "uminus") return SemanticActionId::ACTION_ADD_OP_UNARY_MINUS;
    if (s_action_cue == "u+" || s_action_cue == "uplus") return SemanticActionId::ACTION_ADD_OP_UNARY_PLUS;
    if (s_action_cue == ":=" || s_action_cue == "assign") return SemanticActionId::ACTION_ADD_OP_ASSIGN;
    if (s_action_cue == "==") return SemanticActionId::ACTION_ADD_OP_EQ_CMP;
    if (s_action_cue == "!=") return SemanticActionId::ACTION_ADD_OP_NE;
    if (s_action_cue == "<") return SemanticActionId::ACTION_ADD_OP_LT;
    if (s_action_cue == "<=") return SemanticActionId::ACTION_ADD_OP_LE;
    if (s_action_cue == ">") return SemanticActionId::ACTION_ADD_OP_GT;
    if (s_action_cue == ">=") return SemanticActionId::ACTION_ADD_OP_GE;
    if (s_action_cue == "||") return SemanticActionId::ACTION_ADD_OP_LOGIC_OR;
    if (s_action_cue == "&&") return SemanticActionId::ACTION_ADD_OP_LOGIC_AND;
    if (s_action_cue == "w" || s_action_cue == "s" || s_action_cue == "write" || s_action_cue == "print") return SemanticActionId::ACTION_OP_WRITE_W;
    if (s_action_cue == "r" || s_action_cue == "read" || s_action_cue == "input") return SemanticActionId::ACTION_OP_READ_R;
    if (s_action_cue == "i" || s_action_cue == "idx" || s_action_cue == "index") return SemanticActionId::ACTION_INDEX_ARRAY_I;
    if (s_action_cue == "i2" || s_action_cue == "idx2D") return SemanticActionId::ACTION_INDEX_ARRAY_I2;
    if (s_action_cue == "prog1" || s_action_cue == "P1" || s_action_cue == "1") return SemanticActionId::PROG1_IF_AFTER_COND_GEN_JF;
    if (s_action_cue == "prog2" || s_action_cue == "P2" || s_action_cue == "2") return SemanticActionId::PROG1_IF_AFTER_COND_GEN_JF;
    if (s_action_cue == "prog3" || s_action_cue == "P3" || s_action_cue == "3") return SemanticActionId::PROG3_IF_AFTER_THEN_PATCH_JF_GEN_JUMP;
    if (s_action_cue == "prog4" || s_action_cue == "P4" || s_action_cue == "4") return SemanticActionId::PROG4_IF_AFTER_ELSE_PATCH_JUMP;
    if (s_action_cue == "prog5" || s_action_cue == "P5" || s_action_cue == "5") return SemanticActionId::PROG5_WHILE_BEFORE_COND_PUSH_LABEL;
    if (s_action_cue == "prog_while_jf" ) return SemanticActionId::PROG_WHILE_AFTER_COND_GEN_JF;
    if (s_action_cue == "prog6" || s_action_cue == "P6" || s_action_cue == "6") return SemanticActionId::PROG6_WHILE_AFTER_BODY_GEN_JUMP_PATCH_JF;
    if (s_action_cue == "prog8" || s_action_cue == "P8" || s_action_cue == "8") return SemanticActionId::PROG8_FINALIZE_DECLARATION;
    if (s_action_cue == "prog9" || s_action_cue == "P9" || s_action_cue == "9") return SemanticActionId::PROG9_DEALLOCATE_MEM_END_PROGRAM;
    if (s_action_cue == "prog10" || s_action_cue == "P10" || s_action_cue == "10") return SemanticActionId::PROG10_PROCESS_DECLARED_IDENTIFIER;
    if (s_action_cue == "ЦЕЛЧИС" || s_action_cue == "TYPE_INT") return SemanticActionId::PROG_SET_DECL_TYPE_INT;
    if (s_action_cue == "ВЕЩЧИС" || s_action_cue == "TYPE_DOUBLE") return SemanticActionId::PROG_SET_DECL_TYPE_DOUBLE;
    if (s_action_cue == "ЛОГЧИС" || s_action_cue == "TYPE_BOOL") return SemanticActionId::PROG_SET_DECL_TYPE_BOOL;
    if (s_action_cue == "СТРОКЧИС" || s_action_cue == "TYPE_STRING") return SemanticActionId::PROG_SET_DECL_TYPE_STRING;
    if (s_action_cue == "set_arr_size" || s_action_cue == "ARR_SIZE") return SemanticActionId::ACTION_SET_ARRAY_SIZE;
    if (s_action_cue == "init_arr" || s_action_cue == "INIT_ARR") return SemanticActionId::ACTION_INIT_ARRAY_ITEMS;
    if (s_action_cue == "push_rpn_idx" || s_action_cue == "PUSH_IDX") return SemanticActionId::ACTION_PUSH_RPN_IDX_TO_LABEL_STACK;

    std::cerr << "semanticActionFromString: Неизвестная семантическая метка: " + s_action_cue_full << std::endl;
    throw std::runtime_error("Unknown semantic action cue for table population: '" + s_action_cue_full + "'");
}

void parseRuleString(const std::string& rule_str, NonTerminal nt, ProductionRule& rule_obj) {
    (void)rule_str; (void)nt; (void)rule_obj;
}

void populateParsingTable() {
    ll1_parsing_table.clear();
    ProductionRule rule;
    // P -> print ( S ) ; A Z
    rule.rhs_symbols = {GrammarSymbol(tok_print_kw), GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rparen), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_OP_WRITE_W, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::P_NT][tok_print_kw] = rule;
    // P -> input ( B ) ; A Z
    rule.rhs_symbols = {GrammarSymbol(tok_input_kw), GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::B_NT), GrammarSymbol(tok_rparen), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_OP_READ_R, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::P_NT][tok_input_kw] = rule;
    // P -> int R ; A Z
    rule.rhs_symbols = {GrammarSymbol(tok_int_kw), GrammarSymbol(NonTerminal::R_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG_SET_DECL_TYPE_INT, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::P_NT][tok_int_kw] = rule;
    // P -> double R ; A Z
    rule.rhs_symbols = {GrammarSymbol(tok_double_kw), GrammarSymbol(NonTerminal::R_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG_SET_DECL_TYPE_DOUBLE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::P_NT][tok_double_kw] = rule;
    // P -> bool R ; A Z
    rule.rhs_symbols = {GrammarSymbol(tok_bool_kw), GrammarSymbol(NonTerminal::R_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG_SET_DECL_TYPE_BOOL, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::P_NT][tok_bool_kw] = rule;
    // P -> X ; A Z
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::X_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::P_NT][tok_identifier] = rule;
    // P -> I A Z
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::I_NT), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::P_NT][tok_if_kw] = rule;
    // P -> L A Z
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::L_NT), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::P_NT][tok_while_kw] = rule;
    // P -> Z (for EOF)
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::Z_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::P_NT][tok_eof] = rule;
    // A -> print ( S ) ; A
    rule.rhs_symbols = {GrammarSymbol(tok_print_kw), GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rparen), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_OP_WRITE_W, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::A_NT][tok_print_kw] = rule;
    // A -> input ( B ) ; A
    rule.rhs_symbols = {GrammarSymbol(tok_input_kw), GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::B_NT), GrammarSymbol(tok_rparen), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_OP_READ_R, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::A_NT][tok_input_kw] = rule;
    // A -> int R ; A
    rule.rhs_symbols = {GrammarSymbol(tok_int_kw), GrammarSymbol(NonTerminal::R_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG_SET_DECL_TYPE_INT, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::A_NT][tok_int_kw] = rule;
    // A -> double R ; A
    rule.rhs_symbols = {GrammarSymbol(tok_double_kw), GrammarSymbol(NonTerminal::R_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG_SET_DECL_TYPE_DOUBLE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::A_NT][tok_double_kw] = rule;
    // A -> bool R ; A
    rule.rhs_symbols = {GrammarSymbol(tok_bool_kw), GrammarSymbol(NonTerminal::R_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG_SET_DECL_TYPE_BOOL, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::A_NT][tok_bool_kw] = rule;
    // A -> X ; A
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::X_NT), GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::A_NT][tok_identifier] = rule;
    // A -> I A
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::I_NT), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::A_NT][tok_if_kw] = rule;
    // A -> L A
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::L_NT), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::A_NT][tok_while_kw] = rule;
    // A -> ; A (empty statement)
    rule.rhs_symbols = {GrammarSymbol(tok_semi), GrammarSymbol(NonTerminal::A_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::A_NT][tok_semi] = rule;
    // A -> lambda (for FOLLOW(A) like EOF, else, })
    ProductionRule a_lambda_rule;
    a_lambda_rule.rhs_symbols.clear();
    a_lambda_rule.semantic_actions_for_rhs.clear();
    ll1_parsing_table[NonTerminal::A_NT][tok_eof] = a_lambda_rule;
    ll1_parsing_table[NonTerminal::A_NT][tok_else_kw] = a_lambda_rule;
    ll1_parsing_table[NonTerminal::A_NT][tok_rbrace] = a_lambda_rule;
    // Z -> lambda (for EOF)
    ProductionRule z_lambda_rule;
    z_lambda_rule.rhs_symbols.clear();
    z_lambda_rule.semantic_actions_for_rhs.clear();
    ll1_parsing_table[NonTerminal::Z_NT][tok_eof] = z_lambda_rule;
    // X -> id X'
    rule.rhs_symbols = {GrammarSymbol(tok_identifier), GrammarSymbol(NonTerminal::X_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_IDENT_A, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::X_NT][tok_identifier] = rule;
    // X' -> = S X_ASSIGN_OP_MARKER_NT
    rule.rhs_symbols = {GrammarSymbol(tok_assign), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(NonTerminal::X_ASSIGN_OP_MARKER_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::X_PRIME_NT][tok_assign] = rule;
    // X' -> [ S ] = S X_ASSIGN_OP_MARKER_NT
    rule.rhs_symbols = {GrammarSymbol(tok_lsquare), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rsquare), GrammarSymbol(tok_assign), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(NonTerminal::X_ASSIGN_OP_MARKER_NT)};
    rule.semantic_actions_for_rhs = {
            SemanticActionId::ACTION_NONE,          // [
            SemanticActionId::ACTION_NONE,          // S (index)
            SemanticActionId::ACTION_INDEX_ARRAY_I, // ] (generate index op)
            SemanticActionId::ACTION_NONE,          // =
            SemanticActionId::ACTION_NONE,          // S (value)
            SemanticActionId::ACTION_NONE           // X_ASSIGN_OP_MARKER_NT
    };
    ll1_parsing_table[NonTerminal::X_PRIME_NT][tok_lsquare] = rule;
    // X_ASSIGN_OP_MARKER_NT -> lambda [ACTION_ADD_OP_ASSIGN]
    ProductionRule assign_marker_rule;
    assign_marker_rule.rhs_symbols.clear();
    assign_marker_rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_OP_ASSIGN};
    ll1_parsing_table[NonTerminal::X_ASSIGN_OP_MARKER_NT][tok_semi] = assign_marker_rule;
    // R -> id R'
    rule.rhs_symbols = {GrammarSymbol(tok_identifier), GrammarSymbol(NonTerminal::R_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::PROG10_PROCESS_DECLARED_IDENTIFIER, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::R_NT][tok_identifier] = rule;
    // R' -> [ num ] M
    rule.rhs_symbols = {GrammarSymbol(tok_lsquare), GrammarSymbol(tok_number), GrammarSymbol(tok_rsquare), GrammarSymbol(NonTerminal::M_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_SET_ARRAY_SIZE, SemanticActionId::ACTION_NONE, SemanticActionId::PROG8_FINALIZE_DECLARATION };
    ll1_parsing_table[NonTerminal::R_PRIME_NT][tok_lsquare] = rule;
    // R' -> M (for scalar or next in list after array)
    ProductionRule r_prime_m_rule;
    r_prime_m_rule.rhs_symbols = {GrammarSymbol(NonTerminal::M_NT)};
    r_prime_m_rule.semantic_actions_for_rhs = {SemanticActionId::PROG8_FINALIZE_DECLARATION};
    ll1_parsing_table[NonTerminal::R_PRIME_NT][tok_comma] = r_prime_m_rule;
    ll1_parsing_table[NonTerminal::R_PRIME_NT][tok_semi] = r_prime_m_rule;
    // M -> , id R'
    rule.rhs_symbols = {GrammarSymbol(tok_comma), GrammarSymbol(tok_identifier), GrammarSymbol(NonTerminal::R_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::PROG10_PROCESS_DECLARED_IDENTIFIER, SemanticActionId::ACTION_NONE };
    ll1_parsing_table[NonTerminal::M_NT][tok_comma] = rule;
    // M -> lambda (end of declaration list)
    ProductionRule m_lambda_rule;
    m_lambda_rule.rhs_symbols.clear();
    m_lambda_rule.semantic_actions_for_rhs.clear();
    ll1_parsing_table[NonTerminal::M_NT][tok_semi] = m_lambda_rule;
    // S -> C S'
    const Token first_of_S[] = {tok_plus, tok_minus, tok_lparen, tok_identifier, tok_number, tok_true_val, tok_false_val, tok_string_lit};
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::C_NT), GrammarSymbol(NonTerminal::S_PRIME_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    for (Token t : first_of_S) ll1_parsing_table[NonTerminal::S_NT][t] = rule;
    // S' -> || C S' [||]
    rule.rhs_symbols = {GrammarSymbol(tok_or_logical), GrammarSymbol(NonTerminal::C_NT), GrammarSymbol(NonTerminal::S_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_LOGIC_OR};
    ll1_parsing_table[NonTerminal::S_PRIME_NT][tok_or_logical] = rule;
    // S' -> lambda
    ProductionRule prime_lambda_rule; prime_lambda_rule.rhs_symbols.clear(); prime_lambda_rule.semantic_actions_for_rhs.clear();
    const Token follow_of_S_prime[] = {tok_rparen, tok_semi, tok_rsquare, tok_comma };
    for (Token t : follow_of_S_prime) ll1_parsing_table[NonTerminal::S_PRIME_NT][t] = prime_lambda_rule;
    // C -> T C'
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::T_NT), GrammarSymbol(NonTerminal::C_PRIME_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    for (Token t : first_of_S) ll1_parsing_table[NonTerminal::C_NT][t] = rule;
    // C' -> && T C' [&&]
    rule.rhs_symbols = {GrammarSymbol(tok_and_logical), GrammarSymbol(NonTerminal::T_NT), GrammarSymbol(NonTerminal::C_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_LOGIC_AND};
    ll1_parsing_table[NonTerminal::C_PRIME_NT][tok_and_logical] = rule;
    // C' -> lambda
    const Token follow_of_C_prime[] = {tok_rparen, tok_semi, tok_rsquare, tok_comma, tok_or_logical};
    for (Token t : follow_of_C_prime) ll1_parsing_table[NonTerminal::C_PRIME_NT][t] = prime_lambda_rule;
    // T -> F T'
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::F_NT), GrammarSymbol(NonTerminal::T_PRIME_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    for (Token t : first_of_S) ll1_parsing_table[NonTerminal::T_NT][t] = rule;
    // T' -> == F T' [==] | != F T' [!=] | < F T' [<] etc.
    const std::map<Token, SemanticActionId> compare_op_actions = {
            {tok_eq_cmp, SemanticActionId::ACTION_ADD_OP_EQ_CMP}, {tok_ne, SemanticActionId::ACTION_ADD_OP_NE},
            {tok_lt, SemanticActionId::ACTION_ADD_OP_LT},       {tok_le, SemanticActionId::ACTION_ADD_OP_LE},
            {tok_gt, SemanticActionId::ACTION_ADD_OP_GT},       {tok_ge, SemanticActionId::ACTION_ADD_OP_GE}
    };
    for (auto const& [op_token, op_action_id] : compare_op_actions) {
        rule.rhs_symbols = {GrammarSymbol(op_token), GrammarSymbol(NonTerminal::F_NT), GrammarSymbol(NonTerminal::T_PRIME_NT)};
        rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, op_action_id};
        ll1_parsing_table[NonTerminal::T_PRIME_NT][op_token] = rule;
    }
    // T' -> lambda
    const Token follow_of_T_prime[] = {tok_rparen, tok_semi, tok_rsquare, tok_comma, tok_or_logical, tok_and_logical};
    for (Token t : follow_of_T_prime) ll1_parsing_table[NonTerminal::T_PRIME_NT][t] = prime_lambda_rule;
    // F -> G F'
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::G_NT), GrammarSymbol(NonTerminal::F_PRIME_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    for (Token t : first_of_S) ll1_parsing_table[NonTerminal::F_NT][t] = rule;
    // F' -> + G F' [+]
    rule.rhs_symbols = {GrammarSymbol(tok_plus), GrammarSymbol(NonTerminal::G_NT), GrammarSymbol(NonTerminal::F_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_PLUS};
    ll1_parsing_table[NonTerminal::F_PRIME_NT][tok_plus] = rule;
    // F' -> - G F' [-]
    rule.rhs_symbols = {GrammarSymbol(tok_minus), GrammarSymbol(NonTerminal::G_NT), GrammarSymbol(NonTerminal::F_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_MINUS};
    ll1_parsing_table[NonTerminal::F_PRIME_NT][tok_minus] = rule;
    // F' -> lambda
    const Token follow_of_F_prime[] = {tok_rparen, tok_semi, tok_rsquare, tok_comma, tok_or_logical, tok_and_logical, tok_eq_cmp, tok_ne, tok_lt, tok_le, tok_gt, tok_ge}; // FOLLOW(T') + compare_ops
    for (Token t : follow_of_F_prime) ll1_parsing_table[NonTerminal::F_PRIME_NT][t] = prime_lambda_rule;
    // G -> H G'
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::H_NT), GrammarSymbol(NonTerminal::G_PRIME_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    for (Token t : first_of_S) ll1_parsing_table[NonTerminal::G_NT][t] = rule;
    // G' -> * H G' [*]
    rule.rhs_symbols = {GrammarSymbol(tok_multiply), GrammarSymbol(NonTerminal::H_NT), GrammarSymbol(NonTerminal::G_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_MUL};
    ll1_parsing_table[NonTerminal::G_PRIME_NT][tok_multiply] = rule;
    // G' -> / H G' [/]
    rule.rhs_symbols = {GrammarSymbol(tok_divide), GrammarSymbol(NonTerminal::H_NT), GrammarSymbol(NonTerminal::G_PRIME_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_DIV};
    ll1_parsing_table[NonTerminal::G_PRIME_NT][tok_divide] = rule;
    // G' -> lambda
    const Token follow_of_G_prime[] = {tok_rparen, tok_semi, tok_rsquare, tok_comma, tok_or_logical, tok_and_logical, tok_eq_cmp, tok_ne, tok_lt, tok_le, tok_gt, tok_ge, tok_plus, tok_minus};
    for (Token t : follow_of_G_prime) ll1_parsing_table[NonTerminal::G_PRIME_NT][t] = prime_lambda_rule;
    // H -> + B [u+]
    rule.rhs_symbols = {GrammarSymbol(tok_plus), GrammarSymbol(NonTerminal::B_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_UNARY_PLUS};
    ll1_parsing_table[NonTerminal::H_NT][tok_plus] = rule;
    // H -> - B [u-]
    rule.rhs_symbols = {GrammarSymbol(tok_minus), GrammarSymbol(NonTerminal::B_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_ADD_OP_UNARY_MINUS};
    ll1_parsing_table[NonTerminal::H_NT][tok_minus] = rule;
    // H -> B
    rule.rhs_symbols = {GrammarSymbol(NonTerminal::B_NT)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    const Token first_of_B[] = {tok_lparen, tok_identifier, tok_number, tok_true_val, tok_false_val, tok_string_lit};
    for (Token t : first_of_B) ll1_parsing_table[NonTerminal::H_NT][t] = rule;
    // B -> ( S )
    rule.rhs_symbols = {GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rparen)};
    rule.semantic_actions_for_rhs.assign(rule.rhs_symbols.size(), SemanticActionId::ACTION_NONE);
    ll1_parsing_table[NonTerminal::B_NT][tok_lparen] = rule;
    // B -> id B_ARR_SUFFIX [a]
    rule.rhs_symbols = {GrammarSymbol(tok_identifier), GrammarSymbol(NonTerminal::B_ARR_SUFFIX_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_IDENT_A, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::B_NT][tok_identifier] = rule;
    // B -> num [k]
    rule.rhs_symbols = {GrammarSymbol(tok_number)}; rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_CONST_K};
    ll1_parsing_table[NonTerminal::B_NT][tok_number] = rule;
    // B -> true [k]
    rule.rhs_symbols = {GrammarSymbol(tok_true_val)}; rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_CONST_K};
    ll1_parsing_table[NonTerminal::B_NT][tok_true_val] = rule;
    // B -> false [k]
    rule.rhs_symbols = {GrammarSymbol(tok_false_val)}; rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_CONST_K};
    ll1_parsing_table[NonTerminal::B_NT][tok_false_val] = rule;
    // B -> string_lit [k]
    rule.rhs_symbols = {GrammarSymbol(tok_string_lit)}; rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_ADD_CONST_K};
    ll1_parsing_table[NonTerminal::B_NT][tok_string_lit] = rule;
    // B_ARR_SUFFIX -> [ S ] [idx]
    rule.rhs_symbols = {GrammarSymbol(tok_lsquare), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rsquare)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_INDEX_ARRAY_I};
    ll1_parsing_table[NonTerminal::B_ARR_SUFFIX_NT][tok_lsquare] = rule;
    // B_ARR_SUFFIX -> lambda
    const Token follow_of_B_arr_suffix[] = {tok_multiply, tok_divide, tok_plus, tok_minus, tok_eq_cmp, tok_ne, tok_lt, tok_le, tok_gt, tok_ge, tok_and_logical, tok_or_logical, tok_rparen, tok_semi, tok_rsquare, tok_comma};
    for (Token t : follow_of_B_arr_suffix) ll1_parsing_table[NonTerminal::B_ARR_SUFFIX_NT][t] = prime_lambda_rule;
    // I -> if ( S ) { A } HANDLE_IF_END_MARKER_NT OPTIONAL_ACTUAL_ELSE_NT
    rule.rhs_symbols = {
            GrammarSymbol(tok_if_kw), GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rparen), // cond
            GrammarSymbol(tok_lbrace), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(tok_rbrace),                         // then block
            GrammarSymbol(NonTerminal::HANDLE_IF_END_MARKER_NT),                                                            // marker after then
            GrammarSymbol(NonTerminal::OPTIONAL_ACTUAL_ELSE_NT)                                                             // else part or lambda
    };
    rule.semantic_actions_for_rhs = {
            SemanticActionId::ACTION_NONE,                  // if
            SemanticActionId::ACTION_NONE,                  // (
            SemanticActionId::ACTION_NONE,                  // S
            SemanticActionId::PROG1_IF_AFTER_COND_GEN_JF,   // ) action P1/P2
            SemanticActionId::ACTION_NONE,                  // {
            SemanticActionId::ACTION_NONE,                  // A_NT (statement list in then)
            SemanticActionId::ACTION_NONE,                  // }
            SemanticActionId::ACTION_NONE,                  // HANDLE_IF_END_MARKER_NT (placeholder action for the NT symbol itself)
            SemanticActionId::ACTION_NONE                   // OPTIONAL_ACTUAL_ELSE_NT (placeholder action for the NT symbol itself)
    };
    ll1_parsing_table[NonTerminal::I_NT][tok_if_kw] = rule;
    const Token follow_of_I_NT_for_markers[] = {
            tok_print_kw, tok_input_kw, tok_int_kw, tok_double_kw, tok_bool_kw,
            tok_identifier, tok_if_kw, tok_while_kw, tok_semi,
            tok_eof, tok_rbrace, tok_else_kw
    };
    // HANDLE_IF_END_MARKER_NT -> lambda [PROG3_IF_AFTER_THEN_PATCH_JF_GEN_JUMP]
    ProductionRule handle_if_end_marker_rule;
    handle_if_end_marker_rule.rhs_symbols.clear(); // Lambda
    handle_if_end_marker_rule.semantic_actions_for_rhs = {SemanticActionId::PROG3_IF_AFTER_THEN_PATCH_JF_GEN_JUMP};
    ll1_parsing_table[NonTerminal::HANDLE_IF_END_MARKER_NT][tok_else_kw] = handle_if_end_marker_rule;
    for (Token t : follow_of_I_NT_for_markers) {
        if (t != tok_else_kw) {
            ll1_parsing_table[NonTerminal::HANDLE_IF_END_MARKER_NT][t] = handle_if_end_marker_rule;
        }
    }
    // OPTIONAL_ACTUAL_ELSE_NT -> else E_NT PROG4_ELSE_MARKER_NT
    rule.rhs_symbols = {GrammarSymbol(tok_else_kw), GrammarSymbol(NonTerminal::E_NT), GrammarSymbol(NonTerminal::PROG4_ELSE_MARKER_NT)};
    rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::OPTIONAL_ACTUAL_ELSE_NT][tok_else_kw] = rule;
    // OPTIONAL_ACTUAL_ELSE_NT -> lambda (if no 'else' keyword)
    ProductionRule optional_else_lambda_rule;
    optional_else_lambda_rule.rhs_symbols.clear();
    optional_else_lambda_rule.semantic_actions_for_rhs.clear();
    for (Token t : follow_of_I_NT_for_markers) {
        if (t != tok_else_kw) {
            ll1_parsing_table[NonTerminal::OPTIONAL_ACTUAL_ELSE_NT][t] = optional_else_lambda_rule;
        }
    }
    // E_NT -> { A_NT } (actual else block)
    ProductionRule e_nt_block_rule;
    e_nt_block_rule.rhs_symbols = {GrammarSymbol(tok_lbrace), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(tok_rbrace)};
    e_nt_block_rule.semantic_actions_for_rhs = {SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE, SemanticActionId::ACTION_NONE};
    ll1_parsing_table[NonTerminal::E_NT][tok_lbrace] = e_nt_block_rule;
    // PROG4_ELSE_MARKER_NT -> lambda [PROG4_IF_AFTER_ELSE_PATCH_JUMP]
    ProductionRule prog4_marker_rule;
    prog4_marker_rule.rhs_symbols.clear(); // Lambda
    prog4_marker_rule.semantic_actions_for_rhs = {SemanticActionId::PROG4_IF_AFTER_ELSE_PATCH_JUMP};
    for (Token t : follow_of_I_NT_for_markers) {
        if (t != tok_else_kw) {
            ll1_parsing_table[NonTerminal::PROG4_ELSE_MARKER_NT][t] = prog4_marker_rule;
        }
    }
    // L -> while [P5] ( S ) [P_WHILE_JF] { A } [P6]
    rule.rhs_symbols = {GrammarSymbol(tok_while_kw), GrammarSymbol(tok_lparen), GrammarSymbol(NonTerminal::S_NT), GrammarSymbol(tok_rparen), GrammarSymbol(tok_lbrace), GrammarSymbol(NonTerminal::A_NT), GrammarSymbol(tok_rbrace)};
    rule.semantic_actions_for_rhs = {
            SemanticActionId::PROG5_WHILE_BEFORE_COND_PUSH_LABEL, // Before 'while'
            SemanticActionId::ACTION_NONE,                        // (
            SemanticActionId::ACTION_NONE,                        // S
            SemanticActionId::PROG_WHILE_AFTER_COND_GEN_JF,       // )
            SemanticActionId::ACTION_NONE,                        // {
            SemanticActionId::ACTION_NONE,                        // A_NT
            SemanticActionId::PROG6_WHILE_AFTER_BODY_GEN_JUMP_PATCH_JF // }
    };
    ll1_parsing_table[NonTerminal::L_NT][tok_while_kw] = rule;

    if (ll1_parsing_table.empty()) {
        std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: Таблица LL(1)-разбора пуста после попытки заполнения!" << std::endl;
        throw std::runtime_error("LL(1) parsing table is unexpectedly empty after population attempt.");
    }
    std::cout << "LL(1) parsing table populated. Number of NT entries: " << ll1_parsing_table.size() << std::endl;
}

void executeSemanticAction(SemanticActionId action_id) {
    switch (action_id) {
        case SemanticActionId::ACTION_NONE: { /* Do nothing */ break; }
        case SemanticActionId::ACTION_ADD_IDENT_A: {
            auto it = symbol_table_parser.find(IdentifierStr);
            if (it == symbol_table_parser.end()) {
                throw std::runtime_error("Идентификатор '" + IdentifierStr + "' не объявлен. Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            if (it->second.is_array) {
                addRPN_array_base_id(it->second.id, IdentifierStr, it->second.type);
            } else {
                addRPN_var_id(it->second.id, IdentifierStr, it->second.type);
            }
            break;
        }
        case SemanticActionId::ACTION_ADD_CONST_K: {
            if (CurTok_parser == tok_number) {
                if (NumVal == static_cast<double>(static_cast<long long>(NumVal))) {
                    addRPN_const_int(static_cast<int>(NumVal));
                } else {
                    addRPN_const_double(NumVal);
                }
            } else if (CurTok_parser == tok_string_lit) {
                addRPN_const_string(StringVal);
            } else if (CurTok_parser == tok_true_val) {
                addRPN_const_bool(true);
            } else if (CurTok_parser == tok_false_val) {
                addRPN_const_bool(false);
            } else {
                throw std::runtime_error("ACTION_ADD_CONST_K: необработанный токен для константы: " + std::to_string(CurTok_parser) + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            break;
        }
        case SemanticActionId::ACTION_ADD_OP_PLUS: { addRPN_op(RPNOperationId::OP_ADD); break; }
        case SemanticActionId::ACTION_ADD_OP_MINUS: { addRPN_op(RPNOperationId::OP_SUB); break; }
        case SemanticActionId::ACTION_ADD_OP_MUL: { addRPN_op(RPNOperationId::OP_MUL); break; }
        case SemanticActionId::ACTION_ADD_OP_DIV: { addRPN_op(RPNOperationId::OP_DIV); break; }
        case SemanticActionId::ACTION_ADD_OP_UNARY_MINUS: { addRPN_op(RPNOperationId::OP_UNARY_MINUS); break; }
        case SemanticActionId::ACTION_ADD_OP_UNARY_PLUS: { addRPN_op(RPNOperationId::OP_UNARY_PLUS); break; }
        case SemanticActionId::ACTION_ADD_OP_ASSIGN: { addRPN_op(RPNOperationId::OP_ASSIGN); break; }
        case SemanticActionId::ACTION_ADD_OP_EQ_CMP: { addRPN_op(RPNOperationId::OP_EQ_CMP); break; }
        case SemanticActionId::ACTION_ADD_OP_NE: { addRPN_op(RPNOperationId::OP_NE); break; }
        case SemanticActionId::ACTION_ADD_OP_LT: { addRPN_op(RPNOperationId::OP_LT); break; }
        case SemanticActionId::ACTION_ADD_OP_LE: { addRPN_op(RPNOperationId::OP_LE); break; }
        case SemanticActionId::ACTION_ADD_OP_GT: { addRPN_op(RPNOperationId::OP_GT); break; }
        case SemanticActionId::ACTION_ADD_OP_GE: { addRPN_op(RPNOperationId::OP_GE); break; }
        case SemanticActionId::ACTION_ADD_OP_LOGIC_OR: { addRPN_op(RPNOperationId::OP_LOGICAL_OR); break; }
        case SemanticActionId::ACTION_ADD_OP_LOGIC_AND: { addRPN_op(RPNOperationId::OP_LOGICAL_AND); break; }

        case SemanticActionId::ACTION_OP_WRITE_W: { addRPN_op(RPNOperationId::OP_WRITE_OUTPUT); break; }
        case SemanticActionId::ACTION_OP_READ_R: {
            addRPN_op(RPNOperationId::OP_READ_INPUT);
            addRPN_op(RPNOperationId::OP_ASSIGN, "implicit assign after read");
            break;
        }
        case SemanticActionId::ACTION_INDEX_ARRAY_I: { addRPN_op(RPNOperationId::OP_INDEX_ARRAY); break; }
        case SemanticActionId::ACTION_INDEX_ARRAY_I2: { addRPN_op(RPNOperationId::OP_INDEX_ARRAY_2D); break; }
        case SemanticActionId::PROG1_IF_AFTER_COND_GEN_JF: {
            label_stack_parser.push(addRPN_label_placeholder("IF_JF_Target_PH"));
            addRPN_op(RPNOperationId::OP_JUMP_FALSE);
            break;
        }
        case SemanticActionId::PROG3_IF_AFTER_THEN_PATCH_JF_GEN_JUMP: {
            if (label_stack_parser.empty()) {
                throw std::runtime_error("PROG3: Стек меток пуст для jf! Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            int jf_placeholder_rpn_idx = label_stack_parser.top();

            if (CurTok_parser == tok_else_kw) {
                label_stack_parser.pop();
                label_stack_parser.push(addRPN_label_placeholder("IF_J_Over_ELSE_PH"));
                addRPN_op(RPNOperationId::OP_JUMP);
                patchRPN_label(jf_placeholder_rpn_idx, current_rpn_idx_parser);
            } else {
                patchRPN_label(jf_placeholder_rpn_idx, current_rpn_idx_parser);
                label_stack_parser.pop();
            }
            break;
        }
        case SemanticActionId::PROG4_IF_AFTER_ELSE_PATCH_JUMP: {
            if (label_stack_parser.empty()) {
                throw std::runtime_error("PROG4: Стек меток пуст для jump over else! Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            int j_over_else_placeholder_rpn_idx = label_stack_parser.top(); label_stack_parser.pop();
            patchRPN_label(j_over_else_placeholder_rpn_idx, current_rpn_idx_parser);
            break;
        }
        case SemanticActionId::PROG5_WHILE_BEFORE_COND_PUSH_LABEL: {
            label_stack_parser.push(current_rpn_idx_parser);
            break;
        }
        case SemanticActionId::PROG_WHILE_AFTER_COND_GEN_JF: {
            label_stack_parser.push(addRPN_label_placeholder("WHILE_JF_Target_PH"));
            addRPN_op(RPNOperationId::OP_JUMP_FALSE);
            break;
        }
        case SemanticActionId::PROG6_WHILE_AFTER_BODY_GEN_JUMP_PATCH_JF: {
            if (label_stack_parser.size() < 2) {
                throw std::runtime_error("PROG6: Стек меток не содержит достаточно элементов для WHILE. Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            int jf_placeholder_rpn_idx = label_stack_parser.top(); label_stack_parser.pop();
            int condition_start_rpn_addr = label_stack_parser.top(); label_stack_parser.pop();
            addRPN_raw_label_target(condition_start_rpn_addr);
            addRPN_op(RPNOperationId::OP_JUMP);
            patchRPN_label(jf_placeholder_rpn_idx, current_rpn_idx_parser);
            break;
        }
        case SemanticActionId::PROG_SET_DECL_TYPE_INT: { current_declaration_type = UserVarType::INTEGER; break; }
        case SemanticActionId::PROG_SET_DECL_TYPE_DOUBLE: { current_declaration_type = UserVarType::DOUBLE; break; }
        case SemanticActionId::PROG_SET_DECL_TYPE_BOOL: { current_declaration_type = UserVarType::BOOLEAN; break; }
        case SemanticActionId::PROG_SET_DECL_TYPE_STRING: { current_declaration_type = UserVarType::STRING; break; }
        case SemanticActionId::PROG10_PROCESS_DECLARED_IDENTIFIER: {
            last_declared_identifier_name = IdentifierStr;
            if (symbol_table_parser.count(last_declared_identifier_name)) {
                throw std::runtime_error("Повторное объявление идентификатора: " + last_declared_identifier_name + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            last_declaration_is_array = false;
            last_declaration_array_size = 0;
            break;
        }
        case SemanticActionId::ACTION_SET_ARRAY_SIZE: {
            last_declaration_is_array = true;
            if (NumVal <= 0 || NumVal != static_cast<double>(static_cast<int>(NumVal))) {
                throw std::runtime_error("Размер массива должен быть положительным целым числом: " + std::to_string(NumVal) + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            last_declaration_array_size = static_cast<int>(NumVal);
            break;
        }
        case SemanticActionId::PROG8_FINALIZE_DECLARATION: {
            if (last_declared_identifier_name.empty()) {
                break;
            }
            if (current_declaration_type == UserVarType::UNKNOWN) {
                throw std::runtime_error("PROG8: Тип не установлен для '" + last_declared_identifier_name + "'. Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            SymbolInfo info(last_declared_identifier_name, current_declaration_type,
                            last_declaration_is_array, last_declaration_array_size, next_var_id_parser++);
            symbol_table_parser[last_declared_identifier_name] = info;
            if (info.is_array) {
                addRPN_array_base_id(info.id, info.name, info.type);
                addRPN_const_int(info.array_size);
                addRPN_op(RPNOperationId::OP_ALLOC_MEM_ARRAY, "Alloc array " + info.name, info.type);
            } else {
                addRPN_var_id(info.id, info.name, info.type);       // Push var address (ref)
                addRPN_op(RPNOperationId::OP_ALLOC_MEM_SCALAR, "Alloc scalar " + info.name, info.type);
            }
            last_declared_identifier_name.clear();
            last_declaration_is_array = false;
            last_declaration_array_size = 0;
            break;
        }
        case SemanticActionId::ACTION_INIT_ARRAY_ITEMS: { break; }
        case SemanticActionId::PROG9_DEALLOCATE_MEM_END_PROGRAM: {
            addRPN_op(RPNOperationId::OP_DEALLOC_MEM, "Deallocate all memory at program end");
            break;
        }
        case SemanticActionId::ACTION_PUSH_RPN_IDX_TO_LABEL_STACK: {
            label_stack_parser.push(current_rpn_idx_parser);
            break;
        }
        default: {
            throw std::runtime_error("Неизвестное или нереализованное семантическое действие ID: " + std::to_string(static_cast<int>(action_id)) + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
        }
    }
}

std::string tokenToDebugString(Token t) {
    switch (t) {
        case tok_eof: return "EOF";
        case tok_number: return "число (" + std::to_string(NumVal) + ")";
        case tok_identifier: return "идентификатор (" + IdentifierStr + ")";
        case tok_int_kw: return "'int'";
        case tok_double_kw: return "'double'";
        case tok_bool_kw: return "'bool'";
        case tok_true_val: return "'true'";
        case tok_false_val: return "'false'";
        case tok_eq_cmp: return "'=='";
        case tok_ne: return "'!='";
        case tok_lt: return "'<'";
        case tok_le: return "'<='";
        case tok_gt: return "'>'";
        case tok_ge: return "'>='";
        case tok_and_logical: return "'&&'";
        case tok_or_logical: return "'||'";
        case tok_if_kw: return "'if'";
        case tok_else_kw: return "'else'";
        case tok_while_kw: return "'while'";
        case tok_print_kw: return "'print'";
        case tok_string_lit: return "строковый литерал (\"" + StringVal + "\")";
        case tok_input_kw: return "'input'";
        case tok_plus: return "'+'";
        case tok_minus: return "'-'";
        case tok_multiply: return "'*'";
        case tok_divide: return "'/'";
        case tok_lparen: return "'('";
        case tok_rparen: return "')'";
        case tok_semi: return "';'";
        case tok_assign: return "'='";
        case tok_lbrace: return "'{'";
        case tok_rbrace: return "'}'";
        case tok_lsquare: return "'['";
        case tok_rsquare: return "']'";
        case tok_comma: return "','";
        default:
            if (t > 0 && isprint(t)) {
                return "'" + std::string(1, (char)t) + "'";
            }
            return "неизвестный токен_id(" + std::to_string(static_cast<int>(t)) + ")";
    }
}

void parseProgramLL1() {
    while (!parser_stack.empty()) parser_stack.pop();
    while (!generator_action_stack.empty()) generator_action_stack.pop();
    parser_stack.push(GrammarSymbol(NonTerminal::P_NT));
    generator_action_stack.push(SemanticActionId::ACTION_NONE);
    getNextToken_parser();

    int steps = 0;
    const int MAX_PARSER_STEPS = 1000000; // Safety break for very long/looping parses

    while (!parser_stack.empty() && steps++ < MAX_PARSER_STEPS) {
        SemanticActionId action_to_execute = SemanticActionId::ACTION_NONE;
        if (!generator_action_stack.empty()) {
            action_to_execute = generator_action_stack.top();
        }
        executeSemanticAction(action_to_execute);
        if (!generator_action_stack.empty()) {
            generator_action_stack.pop();
        }
        if (parser_stack.empty()) {
            if (CurTok_parser == tok_eof) {
                bool only_nones_left = true;
                if (!generator_action_stack.empty()) {
                    std::stack<SemanticActionId> temp_check = generator_action_stack;
                    while(!temp_check.empty()){
                        if(temp_check.top() != SemanticActionId::ACTION_NONE) {
                            only_nones_left = false;
                            break;
                        }
                        temp_check.pop();
                    }
                }
                if (!generator_action_stack.empty() && !only_nones_left) {
                    std::string remaining_actions_str;
                    throw std::runtime_error("Парсер: Стек генератора не пуст в конце успешного разбора (содержит не-NONE действия). Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
                } else {
                    while(!generator_action_stack.empty()) generator_action_stack.pop();
                }
            } else {
                throw std::runtime_error("Ошибка: Стек символов пуст, но разбор не завершен. Текущий токен: " + tokenToDebugString(static_cast<Token>(CurTok_parser)) + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
            break;
        }

        GrammarSymbol stack_top_symbol = parser_stack.top();

        if (stack_top_symbol.is_terminal) {
            if (stack_top_symbol.symbol.terminal == CurTok_parser) {
                parser_stack.pop();
                if (CurTok_parser != tok_eof) {
                    getNextToken_parser();
                }
            } else {
                std::string expected_token_str = tokenToDebugString(stack_top_symbol.symbol.terminal);
                std::string actual_token_str = tokenToDebugString(static_cast<Token>(CurTok_parser));
                throw std::runtime_error("Синтаксическая ошибка: ожидался токен " + expected_token_str + ", но получен " + actual_token_str + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }
        } else {
            NonTerminal current_nt = stack_top_symbol.symbol.non_terminal;
            parser_stack.pop();

            auto it_nt_rules = ll1_parsing_table.find(current_nt);
            if (it_nt_rules == ll1_parsing_table.end() || it_nt_rules->second.empty()) {
                throw std::runtime_error("Синтаксическая ошибка: нет правил для нетерминала " + stack_top_symbol.toString() + " в таблице. Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }

            auto it_rule = it_nt_rules->second.find(static_cast<Token>(CurTok_parser));
            if (it_rule == it_nt_rules->second.end()) {
                throw std::runtime_error("Синтаксическая ошибка: нет подходящего правила в таблице разбора для нетерминала " + stack_top_symbol.toString() + " при входном токене " + tokenToDebugString(static_cast<Token>(CurTok_parser)) + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
            }

            const ProductionRule& rule_to_apply = it_rule->second;

            if (rule_to_apply.rhs_symbols.empty()) { // Lambda rule
                if (rule_to_apply.semantic_actions_for_rhs.size() == 1) {
                    executeSemanticAction(rule_to_apply.semantic_actions_for_rhs[0]);
                } else if (rule_to_apply.semantic_actions_for_rhs.size() > 1) {
                    throw std::logic_error("Lambda production has too many semantic actions for NT " + stack_top_symbol.toString() + ". Line " + std::to_string(lineNumber) + ", col " + std::to_string(columnNumber));
                }
            } else {
                for (auto it_sym = rule_to_apply.rhs_symbols.rbegin(); it_sym != rule_to_apply.rhs_symbols.rend(); ++it_sym) {
                    parser_stack.push(*it_sym);
                }
                if (!rule_to_apply.semantic_actions_for_rhs.empty()) {
                    if (rule_to_apply.rhs_symbols.size() != rule_to_apply.semantic_actions_for_rhs.size()) {
                        throw std::logic_error("RHS symbols and actions size mismatch in non-lambda production rule for NT " + std::to_string(static_cast<int>(current_nt)) + ". Line " + std::to_string(lineNumber) + ", col " + std::to_string(columnNumber));
                    }
                    for (auto it_act = rule_to_apply.semantic_actions_for_rhs.rbegin(); it_act != rule_to_apply.semantic_actions_for_rhs.rend(); ++it_act) {
                        generator_action_stack.push(*it_act);
                    }
                } else {
                    for (size_t i = 0; i < rule_to_apply.rhs_symbols.size(); ++i) {
                        generator_action_stack.push(SemanticActionId::ACTION_NONE);
                    }
                }
            }
        }
    }
    if (steps >= MAX_PARSER_STEPS) {
        throw std::runtime_error("Превышен лимит шагов парсера. Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
    }
    if (!parser_stack.empty() || (CurTok_parser != tok_eof) ) {
        std::string stack_state_msg = parser_stack.empty() ? "Стек символов пуст" : "Стек не пуст (вершина: " + parser_stack.top().toString() + ")";
        std::string token_state_msg = (CurTok_parser == tok_eof) ? "токен EOF" : "токен " + tokenToDebugString(static_cast<Token>(CurTok_parser));
        throw std::runtime_error("Ошибка завершения разбора. Состояние: " + stack_state_msg + ", " + token_state_msg + ". Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
    }
    if (!generator_action_stack.empty()) {
        bool only_nones_left_after_loop = true;
        std::stack<SemanticActionId> temp_check = generator_action_stack;
        while(!temp_check.empty()){
            if(temp_check.top() != SemanticActionId::ACTION_NONE) {
                only_nones_left_after_loop = false;
                break;
            }
            temp_check.pop();
        }
        if (!only_nones_left_after_loop) {
            std::string remaining_actions_str;
            throw std::runtime_error("Парсер: Стек генератора не пуст в самом конце разбора (после цикла, содержит не-NONE). Строка " + std::to_string(lineNumber) + ", позиция " + std::to_string(columnNumber));
        } else {
            while(!generator_action_stack.empty()) generator_action_stack.pop();
        }
    }
}


void printRPN_custom() {
    std::cout << "\n--- Сгенерированная ОПЗ (RPN) ---" << std::endl;
    if (rpn_code_parser.empty()) {
        std::cout << "(ОПЗ пуста)" << std::endl;
        return;
    }
    std::cout << "Idx\tЭлемент ОПЗ (Тип:Значение)\t\tКомментарий" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;
    for (size_t i = 0; i < rpn_code_parser.size(); ++i) {
        std::cout << i << ":\t" << rpn_code_parser[i].toString() << std::endl;
    }
}

void resetGlobalStateForNewParse() {
    LastChar = ' ';
    currentLexerState = S_LEX;
    IdentifierStr.clear();
    NumVal = 0.0;
    StringVal.clear();
    BoolVal = false;
    lineNumber = 1;
    columnNumber = 1;

    CurTok_parser = 0;
    rpn_code_parser.clear();
    current_rpn_idx_parser = 0;
    symbol_table_parser.clear();
    next_var_id_parser = 1;
    while(!label_stack_parser.empty()) label_stack_parser.pop();
    while(!parser_stack.empty()) parser_stack.pop();
    while(!generator_action_stack.empty()) generator_action_stack.pop();

    current_declaration_type = UserVarType::UNKNOWN;
    last_declared_identifier_name.clear();
    last_declaration_is_array = false;
    last_declaration_array_size = 0;
}

struct RuntimeValue {
    UserVarType type;
    union ValueUnion {
        int iVal;
        double dVal;
        bool bVal;
        struct MemRef {
            int id;
            int index;
            bool is_array_element_ref;
        } ref;
        ValueUnion() : iVal(0) {}
        ~ValueUnion() {}
    } value;
    std::string sVal_actual;

    RuntimeValue(UserVarType t = UserVarType::UNKNOWN) : type(t) {
        if (t == UserVarType::UNKNOWN) {
            value.ref.id = -1;
            value.ref.index = -1;
            value.ref.is_array_element_ref = false;
        }
    }
    RuntimeValue(int val) : type(UserVarType::INTEGER) { value.iVal = val; }
    RuntimeValue(double val) : type(UserVarType::DOUBLE) { value.dVal = val; }
    RuntimeValue(bool val) : type(UserVarType::BOOLEAN) { value.bVal = val; }
    RuntimeValue(const std::string& val) : type(UserVarType::STRING), sVal_actual(val) {}
    RuntimeValue(int id, bool is_array_base_or_element, int idx = -1) : type(UserVarType::UNKNOWN) {
        value.ref.id = id;
        value.ref.is_array_element_ref = is_array_base_or_element;
        value.ref.index = idx;
    }

    std::string toString() const {
        std::ostringstream oss;
        switch (type) {
            case UserVarType::INTEGER: oss << value.iVal; break;
            case UserVarType::DOUBLE:  oss << value.dVal; break;
            case UserVarType::BOOLEAN: oss << (value.bVal ? "true" : "false"); break;
            case UserVarType::STRING:  oss << "\"" << sVal_actual << "\""; break;
            case UserVarType::UNKNOWN:
                if (value.ref.is_array_element_ref) {
                    if (value.ref.index != -1) {
                        oss << "REF(ArrID:" << value.ref.id << "[" << value.ref.index << "])";
                    } else {
                        oss << "REF(ArrBaseID:" << value.ref.id << ")";
                    }
                } else {
                    oss << "REF(VarID:" << value.ref.id << ")";
                }
                break;
            default: oss << "UNHANDLED_RUNTIME_TYPE";
        }
        return oss.str();
    }
};

class RPNInterpreter {
public:
    std::stack<RuntimeValue> operand_stack;
    std::map<int, RuntimeValue> scalar_memory;
    std::map<int, std::vector<RuntimeValue>> array_memory;
    std::map<int, UserVarType> variable_types;
    std::map<int, int> array_sizes_map;

    void reportRuntimeError(const std::string& message, size_t rpn_idx) {
        std::ostringstream error_message_stream;
        error_message_stream << "Ошибка выполнения (ОПЗ_индекс " << rpn_idx << "): " << message;
        std::cerr << error_message_stream.str() << std::endl;
    }

    RuntimeValue pop_operand() {
        if (operand_stack.empty()) {
            throw std::runtime_error("Стек операндов пуст при попытке извлечения.");
        }
        RuntimeValue val = operand_stack.top();
        operand_stack.pop();
        return val;
    }

    void push_operand(const RuntimeValue& val) {
        operand_stack.push(val);
    }

    RuntimeValue getValueFromMemory(const RuntimeValue::ValueUnion::MemRef& ref) {
        if (variable_types.find(ref.id) == variable_types.end()) {
            throw std::runtime_error("Переменная/массив с ID " + std::to_string(ref.id) + " не объявлен (отсутствует в variable_types).");
        }

        if (ref.is_array_element_ref) {
            if (ref.index == -1) {
                throw std::runtime_error("Попытка разыменования базы массива (ID: " + std::to_string(ref.id) + ") без указания индекса.");
            }
            if (array_memory.find(ref.id) == array_memory.end()) {
                throw std::runtime_error("Массив с ID " + std::to_string(ref.id) + " не найден в array_memory (не аллоцирован?).");
            }
            const auto& arr = array_memory.at(ref.id);
            if (ref.index < 0 || static_cast<size_t>(ref.index) >= arr.size()) {
                throw std::runtime_error("Индекс массива " + std::to_string(ref.index) + " вне границ для массива ID " + std::to_string(ref.id) + " (размер " + std::to_string(arr.size()) + ").");
            }
            return arr[ref.index];
        } else {
            if (scalar_memory.find(ref.id) == scalar_memory.end()) {
                UserVarType type = variable_types.at(ref.id);
                switch(type) {
                    case UserVarType::INTEGER: return RuntimeValue(0);
                    case UserVarType::DOUBLE: return RuntimeValue(0.0);
                    case UserVarType::BOOLEAN: return RuntimeValue(false);
                    case UserVarType::STRING: return RuntimeValue("");
                    default: throw std::runtime_error("Скалярная переменная с ID " + std::to_string(ref.id) + " не найдена и имеет неизвестный тип по умолчанию.");
                }
            }
            return scalar_memory.at(ref.id);
        }
    }

    void storeValueToMemory(const RuntimeValue::ValueUnion::MemRef& ref, RuntimeValue value_to_store) {
        if (variable_types.find(ref.id) == variable_types.end()) {
            throw std::runtime_error("Попытка записи в необъявленную переменную/массив с ID " + std::to_string(ref.id));
        }
        UserVarType expected_type = variable_types.at(ref.id);

        if (expected_type == UserVarType::INTEGER) {
            if (value_to_store.type == UserVarType::DOUBLE) value_to_store = RuntimeValue(static_cast<int>(value_to_store.value.dVal));
            else if (value_to_store.type == UserVarType::BOOLEAN) value_to_store = RuntimeValue(value_to_store.value.bVal ? 1 : 0);
            else if (value_to_store.type != UserVarType::INTEGER) throw std::runtime_error("Несоответствие типов: нельзя присвоить " + value_to_store.toString() + " целому (ID: " + std::to_string(ref.id) + ")");
        } else if (expected_type == UserVarType::DOUBLE) {
            if (value_to_store.type == UserVarType::INTEGER) value_to_store = RuntimeValue(static_cast<double>(value_to_store.value.iVal));
            else if (value_to_store.type == UserVarType::BOOLEAN) value_to_store = RuntimeValue(value_to_store.value.bVal ? 1.0 : 0.0);
            else if (value_to_store.type != UserVarType::DOUBLE) throw std::runtime_error("Несоответствие типов: нельзя присвоить " + value_to_store.toString() + " вещественному (ID: " + std::to_string(ref.id) + ")");
        } else if (expected_type == UserVarType::BOOLEAN) {
            if (value_to_store.type != UserVarType::BOOLEAN) throw std::runtime_error("Несоответствие типов: нельзя присвоить " + value_to_store.toString() + " булевому (ID: " + std::to_string(ref.id) + ")");
        } else if (expected_type == UserVarType::STRING) {
            if (value_to_store.type != UserVarType::STRING) throw std::runtime_error("Несоответствие типов: нельзя присвоить " + value_to_store.toString() + " строке (ID: " + std::to_string(ref.id) + ")");
        }

        if (ref.is_array_element_ref) {
            if (ref.index == -1) {
                throw std::runtime_error("Попытка записи в базу массива (ID: " + std::to_string(ref.id) + ") без указания индекса.");
            }
            if (array_memory.find(ref.id) == array_memory.end()) {
                throw std::runtime_error("Массив с ID " + std::to_string(ref.id) + " не аллоцирован (нет в array_memory).");
            }
            auto& arr = array_memory.at(ref.id);
            if (ref.index < 0 || static_cast<size_t>(ref.index) >= arr.size()) {
                throw std::runtime_error("Индекс массива " + std::to_string(ref.index) + " вне границ для присваивания в массив ID " + std::to_string(ref.id));
            }
            arr[ref.index] = value_to_store;
        } else {
            scalar_memory[ref.id] = value_to_store;
        }
    }

public:
    RPNInterpreter() = default;
    void execute(const std::vector<OPS_Entry>& rpn_code, const std::map<std::string, SymbolInfo>*) {
        if (rpn_code.empty()) {
            std::cout << "Интерпретатор: ОПЗ пуста, нечего выполнять." << std::endl;
            return;
        }

        size_t ip = 0;

        while (ip < rpn_code.size()) {
            const OPS_Entry& current_op_entry = rpn_code[ip];
            try {
                switch (current_op_entry.rpn_val_type) {
                    case RPNValueType::CONSTANT_INT:
                        push_operand(RuntimeValue(current_op_entry.def.ival));
                        break;
                    case RPNValueType::CONSTANT_DOUBLE:
                        push_operand(RuntimeValue(current_op_entry.def.dval));
                        break;
                    case RPNValueType::CONSTANT_BOOL:
                        push_operand(RuntimeValue(current_op_entry.def.bval));
                        break;
                    case RPNValueType::CONSTANT_STRING:
                        push_operand(RuntimeValue(std::string(current_op_entry.def.sval ? current_op_entry.def.sval : "")));
                        break;
                    case RPNValueType::VARIABLE_ID:
                        push_operand(RuntimeValue(current_op_entry.def.ival, false, -1));
                        break;
                    case RPNValueType::ARRAY_BASE_ID:
                        push_operand(RuntimeValue(current_op_entry.def.ival, true, -1));
                        break;
                    case RPNValueType::LABEL_PLACEHOLDER:
                        push_operand(RuntimeValue(current_op_entry.def.ival));
                        break;
                    case RPNValueType::RAW_LABEL_TARGET:
                        push_operand(RuntimeValue(current_op_entry.def.ival));
                        break;
                    case RPNValueType::OPERATION_ID: {
                        RPNOperationId op_code = current_op_entry.def.op_id;
                        switch (op_code) {
                            case RPNOperationId::OP_ADD:
                            case RPNOperationId::OP_SUB:
                            case RPNOperationId::OP_MUL:
                            case RPNOperationId::OP_DIV: {
                                RuntimeValue rv2 = pop_operand();
                                RuntimeValue rv1 = pop_operand();
                                if (rv1.type == UserVarType::UNKNOWN) rv1 = getValueFromMemory(rv1.value.ref);
                                if (rv2.type == UserVarType::UNKNOWN) rv2 = getValueFromMemory(rv2.value.ref);
                                RuntimeValue result;
                                if (op_code == RPNOperationId::OP_ADD && rv1.type == UserVarType::STRING && rv2.type == UserVarType::STRING) {
                                    result = RuntimeValue(rv1.sVal_actual + rv2.sVal_actual);
                                }
                                else if ((rv1.type == UserVarType::INTEGER || rv1.type == UserVarType::DOUBLE || rv1.type == UserVarType::BOOLEAN) &&
                                         (rv2.type == UserVarType::INTEGER || rv2.type == UserVarType::DOUBLE || rv2.type == UserVarType::BOOLEAN)) {

                                    double val1_d, val2_d;
                                    UserVarType result_type_promotion = UserVarType::INTEGER;
                                    if (rv1.type == UserVarType::DOUBLE || rv2.type == UserVarType::DOUBLE) {
                                        result_type_promotion = UserVarType::DOUBLE;
                                    }

                                    val1_d = (rv1.type == UserVarType::INTEGER) ? static_cast<double>(rv1.value.iVal) :
                                             (rv1.type == UserVarType::DOUBLE)  ? rv1.value.dVal :
                                             (rv1.type == UserVarType::BOOLEAN) ? static_cast<double>(rv1.value.bVal ? 1 : 0) : 0.0;
                                    val2_d = (rv2.type == UserVarType::INTEGER) ? static_cast<double>(rv2.value.iVal) :
                                             (rv2.type == UserVarType::DOUBLE)  ? rv2.value.dVal :
                                             (rv2.type == UserVarType::BOOLEAN) ? static_cast<double>(rv2.value.bVal ? 1 : 0) : 0.0;

                                    double num_res_d = 0.0;
                                    if (op_code == RPNOperationId::OP_ADD) num_res_d = val1_d + val2_d;
                                    else if (op_code == RPNOperationId::OP_SUB) num_res_d = val1_d - val2_d;
                                    else if (op_code == RPNOperationId::OP_MUL) num_res_d = val1_d * val2_d;
                                    else if (op_code == RPNOperationId::OP_DIV) {
                                        if (std::abs(val2_d) < 1e-9) throw std::runtime_error("Деление на ноль.");
                                        num_res_d = val1_d / val2_d;
                                        if (result_type_promotion == UserVarType::INTEGER &&
                                            (rv1.type == UserVarType::INTEGER || rv1.type == UserVarType::BOOLEAN) &&
                                            (rv2.type == UserVarType::INTEGER || rv2.type == UserVarType::BOOLEAN) ) {
                                            result = RuntimeValue(static_cast<int>(num_res_d));
                                        } else {
                                            result_type_promotion = UserVarType::DOUBLE;
                                            result = RuntimeValue(num_res_d);
                                        }
                                    }
                                    if (op_code != RPNOperationId::OP_DIV || result.type == UserVarType::UNKNOWN ) {
                                        if (result_type_promotion == UserVarType::INTEGER) result = RuntimeValue(static_cast<int>(num_res_d));
                                        else result = RuntimeValue(num_res_d);
                                    }
                                } else {
                                    throw std::runtime_error("Недопустимые типы для арифметической операции: " + rv1.toString() + ", " + rv2.toString());
                                }
                                push_operand(result);
                                break;
                            }
                            case RPNOperationId::OP_UNARY_MINUS:
                            case RPNOperationId::OP_UNARY_PLUS: {
                                RuntimeValue rv = pop_operand();
                                if (rv.type == UserVarType::UNKNOWN) rv = getValueFromMemory(rv.value.ref);

                                if (op_code == RPNOperationId::OP_UNARY_MINUS) {
                                    if (rv.type == UserVarType::INTEGER) rv = RuntimeValue(-rv.value.iVal);
                                    else if (rv.type == UserVarType::DOUBLE) rv = RuntimeValue(-rv.value.dVal);
                                    else if (rv.type == UserVarType::BOOLEAN) rv = RuntimeValue(-(rv.value.bVal ? 1:0));
                                    else throw std::runtime_error("Унарный минус неприменим к типу " + rv.toString());
                                } else { // Unary Plus
                                    if (rv.type == UserVarType::BOOLEAN) rv = RuntimeValue(+(rv.value.bVal ? 1:0));
                                    else if (rv.type != UserVarType::INTEGER && rv.type != UserVarType::DOUBLE) {
                                        throw std::runtime_error("Унарный плюс неприменим к типу " + rv.toString());
                                    }
                                }
                                push_operand(rv);
                                break;
                            }
                            case RPNOperationId::OP_ASSIGN: {
                                RuntimeValue val_to_assign_rval = pop_operand();
                                RuntimeValue target_lval_ref = pop_operand();

                                if (val_to_assign_rval.type == UserVarType::UNKNOWN) {
                                    val_to_assign_rval = getValueFromMemory(val_to_assign_rval.value.ref);
                                }
                                if (target_lval_ref.type != UserVarType::UNKNOWN) {
                                    throw std::runtime_error("Неверный левый операнд для присваивания (ожидалась ссылка на память), получен " + target_lval_ref.toString());
                                }
                                storeValueToMemory(target_lval_ref.value.ref, val_to_assign_rval);
                                break;
                            }
                            case RPNOperationId::OP_GT: case RPNOperationId::OP_LT:
                            case RPNOperationId::OP_GE: case RPNOperationId::OP_LE:
                            case RPNOperationId::OP_EQ_CMP: case RPNOperationId::OP_NE: {
                                RuntimeValue rv2 = pop_operand(); // RHS
                                RuntimeValue rv1 = pop_operand(); // LHS
                                if (rv1.type == UserVarType::UNKNOWN) rv1 = getValueFromMemory(rv1.value.ref);
                                if (rv2.type == UserVarType::UNKNOWN) rv2 = getValueFromMemory(rv2.value.ref);
                                bool cmp_res = false;
                                if (rv1.type == UserVarType::STRING && rv2.type == UserVarType::STRING) {
                                    if (op_code == RPNOperationId::OP_EQ_CMP) cmp_res = (rv1.sVal_actual == rv2.sVal_actual);
                                    else if (op_code == RPNOperationId::OP_NE) cmp_res = (rv1.sVal_actual != rv2.sVal_actual);
                                    else if (op_code == RPNOperationId::OP_LT) cmp_res = (rv1.sVal_actual < rv2.sVal_actual);
                                    else if (op_code == RPNOperationId::OP_LE) cmp_res = (rv1.sVal_actual <= rv2.sVal_actual);
                                    else if (op_code == RPNOperationId::OP_GT) cmp_res = (rv1.sVal_actual > rv2.sVal_actual);
                                    else if (op_code == RPNOperationId::OP_GE) cmp_res = (rv1.sVal_actual >= rv2.sVal_actual);
                                }
                                else if (rv1.type == UserVarType::BOOLEAN && rv2.type == UserVarType::BOOLEAN) {
                                    if (op_code == RPNOperationId::OP_EQ_CMP) cmp_res = (rv1.value.bVal == rv2.value.bVal);
                                    else if (op_code == RPNOperationId::OP_NE) cmp_res = (rv1.value.bVal != rv2.value.bVal);
                                    else {
                                        double val1_d_bool = rv1.value.bVal ? 1.0 : 0.0;
                                        double val2_d_bool = rv2.value.bVal ? 1.0 : 0.0;
                                        if (op_code == RPNOperationId::OP_GT) cmp_res = val1_d_bool > val2_d_bool;
                                        else if (op_code == RPNOperationId::OP_LT) cmp_res = val1_d_bool < val2_d_bool;
                                        else if (op_code == RPNOperationId::OP_GE) cmp_res = val1_d_bool >= val2_d_bool;
                                        else if (op_code == RPNOperationId::OP_LE) cmp_res = val1_d_bool <= val2_d_bool;
                                    }
                                }
                                else if ((rv1.type == UserVarType::INTEGER || rv1.type == UserVarType::DOUBLE || rv1.type == UserVarType::BOOLEAN) &&
                                         (rv2.type == UserVarType::INTEGER || rv2.type == UserVarType::DOUBLE || rv2.type == UserVarType::BOOLEAN)) {
                                    double val1_d = (rv1.type == UserVarType::INTEGER) ? static_cast<double>(rv1.value.iVal) :
                                                    (rv1.type == UserVarType::DOUBLE)  ? rv1.value.dVal :
                                                    (rv1.type == UserVarType::BOOLEAN) ? (rv1.value.bVal ? 1.0 : 0.0) : 0.0;
                                    double val2_d = (rv2.type == UserVarType::INTEGER) ? static_cast<double>(rv2.value.iVal) :
                                                    (rv2.type == UserVarType::DOUBLE)  ? rv2.value.dVal :
                                                    (rv2.type == UserVarType::BOOLEAN) ? (rv2.value.bVal ? 1.0 : 0.0) : 0.0;
                                    if (op_code == RPNOperationId::OP_GT) cmp_res = val1_d > val2_d;
                                    else if (op_code == RPNOperationId::OP_LT) cmp_res = val1_d < val2_d;
                                    else if (op_code == RPNOperationId::OP_GE) cmp_res = val1_d >= val2_d;
                                    else if (op_code == RPNOperationId::OP_LE) cmp_res = val1_d <= val2_d;
                                    else if (op_code == RPNOperationId::OP_EQ_CMP) cmp_res = (std::abs(val1_d - val2_d) < 1e-9);
                                    else if (op_code == RPNOperationId::OP_NE) cmp_res = (std::abs(val1_d - val2_d) >= 1e-9);
                                } else {
                                    throw std::runtime_error("Несовместимые типы для операции сравнения: " + rv1.toString() + " и " + rv2.toString());
                                }
                                push_operand(RuntimeValue(cmp_res));
                                break;
                            }
                            case RPNOperationId::OP_LOGICAL_AND:
                            case RPNOperationId::OP_LOGICAL_OR: {
                                RuntimeValue rv2 = pop_operand();
                                RuntimeValue rv1 = pop_operand();
                                if (rv1.type == UserVarType::UNKNOWN) rv1 = getValueFromMemory(rv1.value.ref);
                                if (rv2.type == UserVarType::UNKNOWN) rv2 = getValueFromMemory(rv2.value.ref);

                                if (rv1.type != UserVarType::BOOLEAN || rv2.type != UserVarType::BOOLEAN) {
                                    throw std::runtime_error("Логические операции &&, || требуют булевых операндов. Получены: " + rv1.toString() + ", " + rv2.toString());
                                }
                                bool res_b = false;
                                if (op_code == RPNOperationId::OP_LOGICAL_AND) res_b = rv1.value.bVal && rv2.value.bVal;
                                else if (op_code == RPNOperationId::OP_LOGICAL_OR) res_b = rv1.value.bVal || rv2.value.bVal;
                                push_operand(RuntimeValue(res_b));
                                break;
                            }
                            case RPNOperationId::OP_JUMP: {
                                RuntimeValue target_addr_val = pop_operand();
                                if (target_addr_val.type != UserVarType::INTEGER) throw std::runtime_error("Адрес перехода (j) должен быть целым числом.");
                                ip = static_cast<size_t>(target_addr_val.value.iVal);
                                continue;
                            }
                            case RPNOperationId::OP_JUMP_FALSE: {
                                RuntimeValue target_addr_val = pop_operand();
                                RuntimeValue condition_val = pop_operand();

                                if (target_addr_val.type != UserVarType::INTEGER) throw std::runtime_error("Адрес перехода (jf) должен быть целым числом.");
                                if (condition_val.type == UserVarType::UNKNOWN) condition_val = getValueFromMemory(condition_val.value.ref);
                                if (condition_val.type != UserVarType::BOOLEAN) throw std::runtime_error("Условие для jf должно быть булевым, получено " + condition_val.toString());

                                if (!condition_val.value.bVal) {
                                    ip = static_cast<size_t>(target_addr_val.value.iVal);
                                    continue;
                                }
                                break;
                            }
                            case RPNOperationId::OP_ALLOC_MEM_SCALAR: {
                                RuntimeValue var_id_lval = pop_operand();
                                if (var_id_lval.type != UserVarType::UNKNOWN || var_id_lval.value.ref.is_array_element_ref) {
                                    throw std::runtime_error("Ожидался ID скалярной переменной (MemRef) для OP_ALLOC_MEM_SCALAR.");
                                }
                                int var_id = var_id_lval.value.ref.id;
                                UserVarType type_hint = current_op_entry.var_operand_type;
                                variable_types[var_id] = type_hint;
                                RuntimeValue default_val;
                                switch(type_hint) {
                                    case UserVarType::INTEGER: default_val = RuntimeValue(0); break;
                                    case UserVarType::DOUBLE:  default_val = RuntimeValue(0.0); break;
                                    case UserVarType::BOOLEAN: default_val = RuntimeValue(false); break;
                                    case UserVarType::STRING:  default_val = RuntimeValue(""); break;
                                    default: throw std::runtime_error("Неизвестный тип для аллокации скаляра: " + std::to_string(static_cast<int>(type_hint)));
                                }
                                scalar_memory[var_id] = default_val;
                                break;
                            }
                            case RPNOperationId::OP_ALLOC_MEM_ARRAY: {
                                RuntimeValue size_val = pop_operand();
                                RuntimeValue arr_id_lval = pop_operand();
                                if (arr_id_lval.type != UserVarType::UNKNOWN || !arr_id_lval.value.ref.is_array_element_ref || arr_id_lval.value.ref.index != -1) {
                                    throw std::runtime_error("Ожидался ID базы массива (MemRef) для OP_ALLOC_MEM_ARRAY.");
                                }
                                if (size_val.type != UserVarType::INTEGER) throw std::runtime_error("Размер массива должен быть целым числом.");
                                int arr_id = arr_id_lval.value.ref.id;
                                int arr_size = size_val.value.iVal;
                                if (arr_size < 0) throw std::runtime_error("Размер массива не может быть отрицательным.");
                                UserVarType type_hint = current_op_entry.var_operand_type;
                                variable_types[arr_id] = type_hint;
                                array_sizes_map[arr_id] = arr_size;
                                RuntimeValue default_elem_val;
                                switch(type_hint) {
                                    case UserVarType::INTEGER: default_elem_val = RuntimeValue(0); break;
                                    case UserVarType::DOUBLE:  default_elem_val = RuntimeValue(0.0); break;
                                    case UserVarType::BOOLEAN: default_elem_val = RuntimeValue(false); break;
                                    case UserVarType::STRING:  default_elem_val = RuntimeValue(""); break;
                                    default: throw std::runtime_error("Неизвестный тип для аллокации элементов массива: " + std::to_string(static_cast<int>(type_hint)));
                                }
                                array_memory[arr_id].assign(arr_size, default_elem_val);
                                break;
                            }
                            case RPNOperationId::OP_DEALLOC_MEM: {
                                scalar_memory.clear();
                                array_memory.clear();
                                variable_types.clear();
                                array_sizes_map.clear();
                                while(!operand_stack.empty()) operand_stack.pop();
                                std::cout << "Интерпретатор: Память очищена." << std::endl;
                                break;
                            }
                            case RPNOperationId::OP_INDEX_ARRAY: {
                                RuntimeValue index_rval = pop_operand();
                                RuntimeValue arr_base_lval = pop_operand();
                                if (arr_base_lval.type != UserVarType::UNKNOWN || !arr_base_lval.value.ref.is_array_element_ref || arr_base_lval.value.ref.index != -1) {
                                    throw std::runtime_error("Ожидалась ссылка на базу массива для OP_INDEX_ARRAY, получено " + arr_base_lval.toString());
                                }
                                if (index_rval.type == UserVarType::UNKNOWN) index_rval = getValueFromMemory(index_rval.value.ref);
                                if (index_rval.type != UserVarType::INTEGER) throw std::runtime_error("Индекс массива должен быть целым числом, получен " + index_rval.toString());
                                int arr_id = arr_base_lval.value.ref.id;
                                int current_index = index_rval.value.iVal;
                                if (array_memory.find(arr_id) == array_memory.end() ||
                                    static_cast<size_t>(current_index) >= array_sizes_map.at(arr_id) || current_index < 0) {
                                    throw std::runtime_error("Попытка индексации массива ID " + std::to_string(arr_id) +
                                                             " индексом " + std::to_string(current_index) +
                                                             " вне допустимого диапазона [0.." + (array_sizes_map.count(arr_id) ? std::to_string(array_sizes_map.at(arr_id)-1) : "N/A") + "] или массив не аллоцирован.");
                                }
                                push_operand(RuntimeValue(arr_id, true, current_index));
                                break;
                            }
                            case RPNOperationId::OP_READ_INPUT: {
                                if (operand_stack.empty()) throw std::runtime_error("OP_READ_INPUT: Стек операндов пуст, ожидалась ссылка LVal.");
                                RuntimeValue target_lval_ref = operand_stack.top();
                                if (target_lval_ref.type != UserVarType::UNKNOWN) {
                                    throw std::runtime_error("OP_READ_INPUT: Ожидалась ссылка на переменную (MemRef) на вершине стека, получено " + target_lval_ref.toString());
                                }
                                UserVarType read_type = variable_types.at(target_lval_ref.value.ref.id);
                                RuntimeValue read_value(read_type);
                                std::cout << "Введите значение (тип " << static_cast<int>(read_type) << ") для ";
                                if (target_lval_ref.value.ref.is_array_element_ref && target_lval_ref.value.ref.index != -1) {
                                    std::cout << "элемента массива ID " << target_lval_ref.value.ref.id << "[" << target_lval_ref.value.ref.index << "]";
                                } else if (!target_lval_ref.value.ref.is_array_element_ref) {
                                    std::cout << "переменной ID " << target_lval_ref.value.ref.id;
                                } else {
                                    throw std::runtime_error("OP_READ_INPUT: Нельзя вводить значение для базы массива без индекса.");
                                }
                                std::cout << ": ";
                                std::string line_input;
                                if (!std::getline(std::cin, line_input)) {
                                    if (std::cin.eof()) throw std::runtime_error("Ошибка чтения ввода: достигнут конец файла (EOF).");
                                    throw std::runtime_error("Ошибка чтения ввода (ошибка потока).");
                                }
                                if (std::cin.fail() && !std::cin.eof()){
                                    std::cin.clear();
                                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                    throw std::runtime_error("Ошибка потока при вводе (не EOF).");
                                }
                                std::istringstream iss(line_input);
                                try {
                                    if (read_type == UserVarType::INTEGER) {
                                        long long temp_ll;
                                        if (!(iss >> temp_ll) || !iss.eof() || temp_ll > std::numeric_limits<int>::max() || temp_ll < std::numeric_limits<int>::min()) {
                                            throw std::runtime_error("некорректное целое число или переполнение");
                                        }
                                        read_value = RuntimeValue(static_cast<int>(temp_ll));
                                    } else if (read_type == UserVarType::DOUBLE) {
                                        double temp_d;
                                        if (!(iss >> temp_d) || !iss.eof()) throw std::runtime_error("некорректное вещественное число");
                                        read_value = RuntimeValue(temp_d);
                                    } else if (read_type == UserVarType::BOOLEAN) {
                                        std::string bool_str;
                                        if (!(iss >> bool_str) || !iss.eof()) throw std::runtime_error("некорректное булево значение (ожидалась строка 'true'/'false' и т.д.)");
                                        std::transform(bool_str.begin(), bool_str.end(), bool_str.begin(), ::tolower);
                                        if (bool_str == "true" || bool_str == "tr" || bool_str == "1") read_value = RuntimeValue(true);
                                        else if (bool_str == "false" || bool_str == "fls" || bool_str == "0") read_value = RuntimeValue(false);
                                        else throw std::runtime_error("некорректное булево значение (ожидалось true/false/0/1/tr/fls)");
                                    } else if (read_type == UserVarType::STRING) {
                                        read_value = RuntimeValue(line_input);
                                    } else {
                                        throw std::runtime_error("Неподдерживаемый тип для ввода: " + std::to_string(static_cast<int>(read_type)));
                                    }
                                } catch (const std::runtime_error& e_conv) {
                                    throw std::runtime_error("Ошибка преобразования ввода: " + std::string(e_conv.what()));
                                }
                                push_operand(read_value);
                                break;
                            }
                            case RPNOperationId::OP_WRITE_OUTPUT: {
                                RuntimeValue val_to_write = pop_operand();
                                if (val_to_write.type == UserVarType::UNKNOWN) {
                                    val_to_write = getValueFromMemory(val_to_write.value.ref);
                                }
                                std::cout << val_to_write.toString() << std::endl;
                                break;
                            }
                            default:
                                throw std::runtime_error("Выполнение прервано из-за неизвестной операции ОПЗ: " + rpnOpIdToString(op_code) + " на индексе " + std::to_string(ip));
                        }
                        break;
                    }
                    default:
                        throw std::runtime_error("Выполнение прервано из-за неизвестного типа элемента ОПЗ: " + std::to_string(static_cast<int>(current_op_entry.rpn_val_type)) + " на индексе " + std::to_string(ip));
                }
            } catch (const std::runtime_error& e) {
                reportRuntimeError(e.what(), ip);
                std::cerr << "Состояние стека операндов (" << operand_stack.size() << " элементов) во время ошибки:" << std::endl;
                std::stack<RuntimeValue> temp_stack_debug = operand_stack;
                int count_debug = 0;
                while(!temp_stack_debug.empty() && count_debug < 5) {
                    std::cerr << "  Стек[" << count_debug << "]: " << temp_stack_debug.top().toString() << std::endl;
                    temp_stack_debug.pop();
                    count_debug++;
                }
                if (!temp_stack_debug.empty()) std::cerr << "  ..." << std::endl;
                throw;
            }
            ip++;
        }
        if (ip == rpn_code.size()) {
            std::cout << "Интерпретатор: Выполнение ОПЗ завершено успешно." << std::endl;
            if (!operand_stack.empty()) {
                std::cout << "Предупреждение: Стек операндов не пуст в конце выполнения (" << operand_stack.size() << " элементов осталось)." << std::endl;
            }
        }
    }
};

int main() {
    std::cout << "Программа LL(1) синтаксического анализатора, генератора и интерпретатора ОПЗ." << std::endl;
    std::string full_input_for_parser;
    std::string line_buffer;

    std::cout << "Введите ваш код. Для завершения ввода введите '###END###' на новой строке и нажмите Enter:" << std::endl;
    while (std::getline(std::cin, line_buffer)) {
        if (line_buffer == "###END###") break;
        full_input_for_parser += line_buffer + "\n";
    }

    if (full_input_for_parser.empty()) {
        std::cout << "Ввод не содержит кода для анализа. Завершение программы." << std::endl;
        return 0;
    }
    if (!full_input_for_parser.empty() && full_input_for_parser.back() != '\n') {
        full_input_for_parser += '\n';
    }

    std::istringstream code_stream(full_input_for_parser);
    resetGlobalStateForNewParse();
    InputStream = &code_stream;

    std::cout << "\n--- Попытка LL(1) синтаксического анализа и генерации ОПЗ: ---\n";
    try {
        populateParsingTable();
        parseProgramLL1();
        executeSemanticAction(SemanticActionId::PROG9_DEALLOCATE_MEM_END_PROGRAM);

        std::cout << "\nСинтаксический анализ и генерация ОПЗ успешно завершены.\n";
        printRPN_custom();

        std::cout << "\n--- Попытка выполнения ОПЗ: ---" << std::endl;
        RPNInterpreter interpreter;
        interpreter.execute(rpn_code_parser, &symbol_table_parser);

    } catch (const std::runtime_error& e) {
        std::cerr << "\nИСКЛЮЧЕНИЕ (runtime_error) ПОЙМАНО В MAIN: " << e.what() << std::endl;
        if (!rpn_code_parser.empty() && CurTok_parser != tok_eof){
            std::cout << "\n--- Текущее состояние ОПЗ при ошибке: ---" << std::endl;
            printRPN_custom();
        }
        return 1;
    } catch (const std::logic_error& e) {
        std::cerr << "\nВНУТРЕННЯЯ ЛОГИЧЕСКАЯ ОШИБКА (logic_error) ПОЙМАНА В MAIN: " << e.what() << std::endl;
        if (!rpn_code_parser.empty()){
            std::cout << "\n--- Текущее состояние ОПЗ при ошибке: ---" << std::endl;
            printRPN_custom();
        }
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\nОБЩАЯ ОШИБКА (std::exception) ПОЙМАНА В MAIN: " << e.what() << std::endl;
        if (!rpn_code_parser.empty()){
            std::cout << "\n--- Текущее состояние ОПЗ при ошибке: ---" << std::endl;
            printRPN_custom();
        }
        return 1;
    } catch (...) {
        std::cerr << "\nНЕИЗВЕСТНАЯ КРИТИЧЕСКАЯ ОШИБКА ПОЙМАНА В MAIN!" << std::endl;
        if (!rpn_code_parser.empty()){
            std::cout << "\n--- Текущее состояние ОПЗ при ошибке: ---" << std::endl;
            printRPN_custom();
        }
        return 1;
    }
    return 0;
}