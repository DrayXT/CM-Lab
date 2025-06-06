#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <stack>
#include <sstream>
#include <unordered_set>

using namespace std;

// ДЛЯ ПРАВОЙ ЧАСТИ ПРАВИЛА
// Терминалы (1-27)
#define INT      1    // int
#define REAL     2    // real
#define IF       3    // if
#define WHILE    4    // while
#define INPUT    5    // input
#define OUTPUT   6    // output
#define ID       7    // идентификатор
#define NUM      8    // константа
#define LPAREN   9    // (
#define COMMA    10   // ,
#define ELSE     11   // else
#define LBRACE   12   // {
#define EQ       13   // ==
#define NOT      14   // !
#define LESS     15   // <
#define GREATER  16   // >
#define PLUS     17   // +
#define MINUS    18   // -
#define MULT     19   // *
#define DIV      20   // /
#define LBRACKET 21   // [
#define SEMICOL  22   // ;
#define ASSIGN   23   // =
#define RBRACKET 24   // ]
#define RBRACE   25   // }
#define RPAREN   26   // )

// Нетерминалы (начиная с 28)
#define PROGRAM  28   // <Программа>
#define STMT_LIST 29  // <Список операторов>
#define VAR_DECL 30  // <Объявление переменной>
#define VAR_DECL_CONT 31  // <Объявление продолжение>
#define ARRAY_DECL_CONT 32  // <Объявление массива продолжение>
#define ALT_ACTION 33  // <Альтернативное действие>
#define BLOCK    34   // <Блок>
#define ASSIGN_CONT 35  // <Присваивание продолжение>
#define ARRAY_ASSIGN_CONT 36  // <Присваивание массива продолжение>
#define INPUT_CONT 37  // <Ввод с консоли продолжение>
#define OUTPUT_CONT 38  // <Вывод в консоль продолжение>
#define COMPARISON 39  // <Проверка равенства>
#define EXPR     40   // <Выражение>
#define EXPR_CONT 41  // <Выражение продолжение>
#define TERM     42   // <Терм>
#define TERM_CONT 43  // <Терм продолжение>
#define FACTOR   44   // <Фактор>
#define FACTOR_CONT 45  // <Фактор продолжение>
#define INITIALIZERS 46  // <Инициализаторы>
#define INITIALIZERS_CONT 47  // <Инициализаторы продолжение>
#define FOR_COMPARISON 48 // <для сравнений>
#define OUTPUT_ARRAY 49 // <Вывод массива>
#define INPUT_ARRAY 50 // <Ввод массива>

// Специальные символы
#define NONE     -1   // другой символ
#define EOS      '\0' // конец строки
#define EOF      -2 // конец файла
#define LAMBDA   0    // пустая строка


//таблица лексического анализатора, номера семантических программ
int lexTable[6][22] = {
    //	   a-z 0-9  +   -   =   *   /  ' '  (   )  [    ]  {    }  <    >   !   ;  \n  . other EOF
    //      0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21
    /*S*/{  1,  2,  3,  4,  5,  6,  7,  8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 21, 20, 29, 23 },
    /*A*/{ 24, 24, 25, 25, 25, 25, 25, 25, 22, 25, 25, 25, 25, 25, 25, 25, 25, 25, 21, 22, 22, 23 },
    /*B*/{ 22, 26, 27, 27, 27, 27, 27, 27, 22, 27, 22, 27, 22, 27, 27, 27, 27, 27, 21, 20, 22, 23 },
    /*C*/{ 22, 28, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
    /*D*/{ 22, 28, 30, 30, 30, 30, 30, 30, 22, 30, 22, 30, 22, 22, 30, 30, 30, 30, 30, 22, 22, 30 },
    /*E*/{ 25, 25, 25, 25,  9, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 22, 22, 22, 22 }
};

int stateTable[6][22] = {
    //	   a-z 0-9  +    -   =   *   /  ' '  (   )  [     ]  {    }  <    >   !    ;  \n   .  other  EOF
    //      0   1   2    3   4   5   6   7   8   9  10   11  12  13  14  15   16   17  18  19  20    21
    /*S*/{  2,  3,  1,   1,  8,  1,  1,  0,  1,  1,  1,   1,  1,  1,  1,  1,   1,  1,  0,  1,  6,    1 },
    /*A*/{  2,  2,  7,   7,  7,  7,  7,  1,  6,  7,  7,   7,  7,  7,  7,  7,   7,  7,  1,  6,  6,    1 },
    /*B*/{  6,  3,  7,   7,  7,  7,  7,  1,  6,  7,  6,   7,  6,  7,  7,  7,   7,  7,  1,  4,  6,    1 },
    /*C*/{  6,  5,  6,   6,  6,  6,  6,  6,  6,  6,  6,   6,  6,  6,  6,  6,   6,  6,  6,  6,  6,    6 },
    /*D*/{  6,  5,  7,   7,  7,  7,  7,  7,  6,  7,  6,   7,  6,  6,  7,  7,   7,  7,  1,  6,  6,    1 },
    /*E*/{  7,  7,  7,   7,  1,  7,  7,  7,  7,  7,  7,   7,  7,  7,  7,  7,   7,  7,  6,  6,  6,    6 }
};

// состояние ЛА может принимать значения:
// 0 = S - начальное состояние,
// Промежуточные состояния:
// 2 = A - чтение букв,
// 3 = B - чтение цифр,
// 4 = C - чтение десятичного разделителя,
// 5 = D - чтение десятичной части,
// 7 = F* - переход в заключительное состояние с действием i:=i-1,
// 8 = E - чтение оператора сравнения,
// 1 = F - правильное конечное,
// 6 = Z - ошибочное конечное	


//      Семантические программы:
//1	    Начало лексемы name:=C[i]
//2	    Начало числа n:= ord(C[i]) - ord('0')
//3	    Распознано сложение
//4	    Распознано вычитание
//5	    Распознано присваивание
//6	    Распознано умножение
//7	    Распознано деление
//8	    Распознан пробел
//9	    Распознано сравнение
//10	Распознан (
//11	Распознан )
//12	Распознан [
//13	Распознан ]
//14	Распознан {
//15	Распознан }
//16	Распознан <
//17	Распознан >
//18	Распознан !
//19	Распознан ;
//20	Распознан разделитель десятич. части d := 1, x := n
//21	Распознан переход на новую строку
//22	Ошибочная лексема
//23	Конец файла
//24	Продолжение лексемы name := name + C[i]
//25	Распознана лексема
//26	Продолжение числа n := n * 10 + ord(C[i]) - ord('0')
//27	Число распознано
//28	Чтение десятичной части d := d * 0.1; x := x + (ord(C[i]) - ord('0')) * d
//29	Не символ языка
//30	Вещественное число распознано


//таблица переходов, по ASCI-коду символа получаем номер столбца в lexTab
//номер столбца в лекстабле,
//код символа есть индекс массива
int AsciiTable[128] = {
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, //0 - 9
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, //10 - 19
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, //20 - 29
    20, 20,  7, 16, 20, 20, 20, 20, 20, 20, //30 - 39
     8,  9,  5,  2, 20,  3, 19,  6,  1,  1, //40 - 49
     1,  1,  1,  1,  1,  1,  1,  1, 20, 17, //50 - 59
    14,  4, 15, 20, 20,  0,  0,  0,  0,  0, //60 - 69
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //70 - 79
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //80 - 89
     0, 10, 20, 11, 20, 20, 20,  0,  0,  0, //90 - 99
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //100 - 109
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //110 - 119
     0,  0,  0, 12, 20, 13, 20, 20          //120 - 127
};

enum class TokenType {
    Keyword,
    Identifier,
    Integer,
    Real,
    Plus,
    Minus,
    Multiply,
    Divide,
    Assign, // =
    Comparison, // ==
    LeftParen, // (
    RightParen, // )
    LeftBracket, // [
    RightBracket, // ]
    LeftBrace, // {
    RightBrace, // }
    Less, // <
    Greater, // >
    Exclamation, // !
    Semicolon, // ;
    EndOfFile,
    Unknown // @, #, ...
};

struct Token {
    TokenType type;
    int numLexeme;
    string lexeme;
    float realValue;
    int intValue;
    int line; // номер строки в исходнике (для отладки и синтакс. анализа)
    int currLinePos; // позиция в строке
};

vector<Token> tokens;
int currentLine = 1;

string toString(TokenType type);

// Глобальные переменные
string name = "";
int n = 0;
float x = 0.0;
float d = 1.0;

// ******************************* LEXICAL ANALIZATOR *******************************
void lexicalAnalyzer(const string& fileName) {
    ifstream file(fileName);
    if (!file) {
        cerr << "Ошибка открытия файла!" << endl;
        return;
    }

    char c;
    int state = 0;
    int col, semantic;
    currentLine = 1;
    int currentChar = 0;

    string currentLineContent;
    tokens.clear();

    while (file.get(c)) {
        if (c == '\n') {
            ++currentLine;
            currentChar = 0;
            currentLineContent.clear();
            continue;
        }

        ++currentChar;
        currentLineContent += c;

        col = (c >= 0 && c < 128) ? AsciiTable[(int)c] : 20;
        semantic = lexTable[state][col];

        switch (semantic) {
        case 1: name = c; break;
        case 2:
            n = c - '0';
            break;
        case 3: tokens.push_back({ TokenType::Plus, PLUS, "+", 0.0f, 0, currentLine, currentChar }); break;
        case 4:
            tokens.push_back({ TokenType::Minus, MINUS, "-", 0.0f, 0, currentLine, currentChar });
            break;
        case 5: tokens.push_back({ TokenType::Assign, ASSIGN, "=", 0.0f, 0, currentLine, currentChar }); break;
        case 6: tokens.push_back({ TokenType::Multiply, MULT, "*", 0.0f, 0, currentLine, currentChar }); break;
        case 7: tokens.push_back({ TokenType::Divide, DIV, "/", 0.0f, 0, currentLine, currentChar }); break;
        case 8: break; // пробел пропускаем
        case 9:
            tokens.pop_back();
            tokens.push_back({ TokenType::Comparison, EQ, "==", 0.0f, 0, currentLine, currentChar });
            break;
        case 10: tokens.push_back({ TokenType::LeftParen, LPAREN, "(", 0.0f, 0, currentLine, currentChar }); break;
        case 11: tokens.push_back({ TokenType::RightParen, RPAREN, ")", 0.0f, 0, currentLine, currentChar }); break;
        case 12: tokens.push_back({ TokenType::LeftBracket, LBRACKET, "[", 0.0f, 0, currentLine, currentChar }); break;
        case 13: tokens.push_back({ TokenType::RightBracket, RBRACKET, "]", 0.0f, 0, currentLine, currentChar }); break;
        case 14: tokens.push_back({ TokenType::LeftBrace, LBRACE, "{", 0.0f, 0, currentLine, currentChar }); break;
        case 15: tokens.push_back({ TokenType::RightBrace, RBRACE, "}", 0.0f, 0, currentLine, currentChar }); break;
        case 16: tokens.push_back({ TokenType::Less, LESS, "<", 0.0f, 0, currentLine, currentChar }); break;
        case 17: tokens.push_back({ TokenType::Greater, GREATER, ">", 0.0f, 0, currentLine, currentChar }); break;
        case 18: tokens.push_back({ TokenType::Exclamation, NOT, "!", 0.0f, 0, currentLine, currentChar }); break;
        case 19: tokens.push_back({ TokenType::Semicolon, SEMICOL, ";", 0.0f, 0, currentLine, currentChar }); break;
        case 20: d = 0.1; x = n; break;
        case 21: break;
        case 22:
            cerr << "\nРанее распознанные токены:\n";
            for (const auto& token : tokens) {
                cout << "Token: " << toString(token.type)
                    << ", Lexeme: \"" << token.lexeme
                    << "\", LexemeNUM: " << token.numLexeme
                    << ", Line: " << token.line << endl;
                if (token.intValue != 0) cout << "Token: " << toString(token.type) << ", Value: " << token.intValue << endl;
                if (token.realValue != 0) cout << "Token: " << toString(token.type) << ", Value: " << token.realValue << endl;
            }
            cerr << "\nОшибка: неизвестный символ '" << c
                << "' в строке " << currentLine
                << ", позиция " << currentChar << endl;
            cerr << "Строка: " << currentLineContent << endl;
            return;
        case 23: tokens.push_back({ TokenType::EndOfFile, EOF, "EOF", 0.0f, 0, currentLine, currentChar }); break;
        case 24: name += c; break;
        case 25:
            if (!name.empty()) {

                static const unordered_map<string, int> keywords = {
                    {"int", INT},
                    {"real", REAL},
                    {"if", IF},
                    {"else", ELSE},
                    {"while", WHILE},
                    {"input", INPUT},
                    {"output", OUTPUT}
                };

                auto it = keywords.find(name);
                if (it != keywords.end()) {
                    tokens.push_back({ TokenType::Keyword, it->second, name, 0.0f, 0, currentLine, currentChar });
                }
                else {
                    tokens.push_back({ TokenType::Identifier, ID, name, 0.0f, 0, currentLine, currentChar });
                }
                name.clear();
            }
            break;
        case 26: n = n * 10 + (c - '0'); break;
        case 27:
            tokens.push_back({ TokenType::Integer, NUM, to_string(n), 0.0f, n, currentLine, currentChar });
            break;
        case 28: x += (c - '0') * d; d *= 0.1; break;
        case 30:
            tokens.push_back({ TokenType::Real, NUM, to_string(x), x, 0, currentLine, currentChar });
            break;
        }

        state = stateTable[state][col];
        if (state == 2) state = 1;
        else if (state == 3) state = 2;
        else if (state == 4) state = 3;
        else if (state == 5) state = 4;
        else if (state == 8) state = 5;
        else state = 0;
    }

    tokens.push_back({ TokenType::EndOfFile, EOF, "EOF", 0.0f, 0, currentLine, currentChar });
}

void printLexAnalResult(vector<Token>& tokens)
{
    for (const auto& token : tokens) {
        cout << "Token: " << toString(token.type)
            << ", Lexeme: \"" << token.lexeme
            << "\", LexemeNUM: " << token.numLexeme
            << ", Line: " << token.line << endl;
        if (token.intValue != 0) cout << "Token: " << toString(token.type) << ", Value: " << token.intValue << endl;
        if (token.realValue != 0) cout << "Token: " << toString(token.type) << ", Value: " << token.realValue << endl;
    }
}

string toString(TokenType type) {
    switch (type) {
    case TokenType::Keyword: return "Keyword";
    case TokenType::Identifier: return "Identifier";
    case TokenType::Integer: return "Integer";
    case TokenType::Real: return "Real";
    case TokenType::Plus: return "Plus";
    case TokenType::Minus: return "Minus";
    case TokenType::Multiply: return "Multiply";
    case TokenType::Divide: return "Divide";
    case TokenType::Assign: return "Assign";
    case TokenType::Comparison: return "Comparison";
    case TokenType::LeftParen: return "LeftParen";
    case TokenType::RightParen: return "RightParen";
    case TokenType::LeftBracket: return "LeftBracket";
    case TokenType::RightBracket: return "RightBracket";
    case TokenType::LeftBrace: return "LeftBrace";
    case TokenType::RightBrace: return "RightBrace";
    case TokenType::Less: return "Less";
    case TokenType::Greater: return "Greater";
    case TokenType::Exclamation: return "Exclamation";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::EndOfFile: return "EndOfFile";
    case TokenType::Unknown: return "Unknown";
    default: return "Unknown";
    }
}
// ******************************* LEXICAL ANALIZATOR *******************************

void fixUnaryMinus(vector<Token>& tokens) {
    // Список лексем, после которых минус считается унарным
    unordered_set<string> unaryBefore = { "=", "<", ">", "(", "[", "!" };

    for (size_t i = 1; i + 1 < tokens.size(); ++i) {
        // Минус на позиции i
        if (tokens[i].type == TokenType::Minus) {
            const string& prevLexeme = tokens[i - 1].lexeme;

            // Проверка: перед ним один из операторов или скобок
            if (unaryBefore.count(prevLexeme)) {
                Token& next = tokens[i + 1];

                // Если после минуса число — делаем отрицательным
                if (next.type == TokenType::Integer) {
                    next.intValue *= -1;
                    next.lexeme = "-" + next.lexeme;
                    tokens.erase(tokens.begin() + i); // удаляем минус
                }
                // Если после минуса переменная — просто добавим "-" к имени (опционально)
                else if (next.type == TokenType::Identifier) {
                    next.intValue *= -1;
                    // next.lexeme = "-" + next.lexeme;
                    tokens.erase(tokens.begin() + i); // удаляем минус
                }
                // Иначе — ничего не делаем (оставим как есть)
            }
        }
    }
}


// ******************************* SEMANTIC ANALIZATOR *******************************
// ДЛЯ СЕМАНТИЧЕСКИХ ДЕЙСТВИЙ
enum class SemanticActionType {
    // Базовые операции
    EMPTY_ACTION,      // Пустое действие
    PUSH_ID,           // a - записать идентификатор в ОПС
    PUSH_CONST,        // k - записать константу в ОПС
    WRITE_OP,          // w - оператор записи
    READ_OP,           // r - оператор чтения
    ARRAY_INIT,        // init - инициализация массива
    ARRAY_GEN,         // GEN - генерация массива
    JUMP,              // j - безусловный переход
    COND_JUMP_FALSE,   // jf - переход если false
    ARRAY_INDEX,       // i - индексация массива

    // Арифметические операции
    ADD_OP,            // +
    SUB_OP,            // -
    MUL_OP,            // *
    DIV_OP,            // /

    // Операции сравнения
    LESS_OP,           // <
    GREATER_OP,        // >
    NOT_OP,            // !
    EQUAL_OP,          // ==

    ASSIGN_OP, // =

    // Комплексные программы
    PROG_1,  // Программа 1
    PROG_2,  // Программа 2
    PROG_3,  // Программа 3
    PROG_4,  // Программа 4
    PROG_5,  // Программа 5
    PROG_6,  // Программа 6
    PROG_7,  // Программа 7
    PROG_8,  // Программа 8
    PROG_9,  // Программа 9
    PROG_10,  // Программа 10
    PROG_11,  // Программа 11
    PROG_12  // Программа 12
};

// ЯЧЕЙКА СЕМАНТИЧЕСКОЙ ТАБЛИЦЫ
struct Rule {
    vector <int> rightRule;
    vector <SemanticActionType> semanticActions;
};

// СЕМАНТИЧЕСКАЯ ТАБЛИЦА
// Пример заполнения таблицы (21 строк x 24 столбца)
Rule semanticTable[23][26] = {
    // Строка "Программа"
    {
        /* Столбец "int" */             {{INT, VAR_DECL, STMT_LIST}, {SemanticActionType::PROG_1, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // int <Объявление переменной><Список операторов> | 1 [] []
        /* Столбец "real" */            {{REAL, VAR_DECL, STMT_LIST}, {SemanticActionType::PROG_1, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // real <Объявление переменной> <Список операторов> | 1 [] [] 
        /* Столбец "if" */              {{IF, LPAREN, EXPR, COMPARISON, RPAREN, BLOCK, ALT_ACTION, FOR_COMPARISON}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_9, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_11}}, // if ( <Выражение><Проверка равенства> ) <Блок> <Альтернативное действие> <Для сравнений> | [][][][] 9 [][] 10
        /* Столбец "while" */           {{WHILE, LPAREN, EXPR, COMPARISON, RPAREN, BLOCK, FOR_COMPARISON}, {SemanticActionType::PROG_6, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_7, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_8}}, // while ( <Выражение><Проверка равенства> ) <Блок> <Для сравнений> | 6[][][] 7 []8
        /* Столбец "input" */           {{INPUT, INPUT_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // input <Ввод с консоли продолжение> | [] []
        /* Столбец "output" */          {{OUTPUT, OUTPUT_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // output <Вывод в консоль продолжение> | [] []
        /* Столбец "идентификатор" */   {{ID, ASSIGN_CONT, STMT_LIST}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Присваивание продолжение> <Список операторов>| a [][]
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Список операторов"
    {
        /* Столбец "int" */             {{INT, VAR_DECL, STMT_LIST}, {SemanticActionType::PROG_1, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // int <Объявление переменной><Список операторов> | 1 [] []
        /* Столбец "real" */            {{REAL, VAR_DECL, STMT_LIST}, {SemanticActionType::PROG_1, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // real <Объявление переменной> <Список операторов> | 1 [] [] 
        /* Столбец "if" */              {{IF, LPAREN, EXPR, COMPARISON, RPAREN, BLOCK, ALT_ACTION, FOR_COMPARISON, STMT_LIST}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_9, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_11, SemanticActionType::EMPTY_ACTION}}, // if ( <Выражение><Проверка равенства> ) <Блок> <Альтернативное действие> <Для сравнений> | [][][][] 9 [][] 10
        /* Столбец "while" */           {{WHILE, LPAREN, EXPR, COMPARISON, RPAREN, BLOCK, FOR_COMPARISON, STMT_LIST}, {SemanticActionType::PROG_6, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_7, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_8, SemanticActionType::EMPTY_ACTION}}, // while ( <Выражение><Проверка равенства> ) <Блок> <Для сравнений> | 6[][][] 7 []8
        /* Столбец "input" */           {{INPUT, INPUT_CONT, STMT_LIST}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // input <Ввод с консоли продолжение> | [] []
        /* Столбец "output" */          {{OUTPUT, OUTPUT_CONT, STMT_LIST}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // output <Вывод в консоль продолжение> | [] [] []
        /* Столбец "идентификатор" */   {{ID, ASSIGN_CONT, STMT_LIST}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Присваивание продолжение> <Список операторов>| a [][]
        /* Столбец "константа" */       {{LAMBDA}, {}}, // lambda
        /* Столбец "(" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "," */               {{LAMBDA}, {}}, // lambda
        /* Столбец "else" */            {{LAMBDA}, {}}, // lambda
        /* Столбец "{" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "==" */              {{LAMBDA}, {}}, // lambda
        /* Столбец "!" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "<" */               {{LAMBDA}, {}}, // lambda
        /* Столбец ">" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "+" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "-" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "*" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "/" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "[" */               {{LAMBDA}, {}}, // lambda
        /* Столбец ";" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "=" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "]" */               {{LAMBDA}, {}}, // lambda
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "Объявление переменной"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, VAR_DECL_CONT}, {SemanticActionType::PROG_3, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Объявление продолжение> | a[]
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{LBRACKET, ARRAY_DECL_CONT}, {SemanticActionType::PROG_2, SemanticActionType::EMPTY_ACTION}}, // [ <Объявление массива продолжение> | [][]
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Объявление продолжение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{}, {}},
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{SEMICOL}, {SemanticActionType::EMPTY_ACTION}}, // ; | []       
        /* Столбец "=" */               {{ASSIGN, EXPR, SEMICOL}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ASSIGN_OP}}, // =<Выражение> ; | [] []=      
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Объявление массива продолжение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, FACTOR_CONT, EXPR_CONT}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Фактор продолжение> <Выражение продолжение> | a[][]
        /* Столбец "константа" */       {{NUM, EXPR_CONT, RBRACKET, ID, SEMICOL}, {SemanticActionType::PROG_2, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::PROG_12, SemanticActionType::EMPTY_ACTION}}, // число <Выражение продолжение> ] идентификатор | 2[]i 12[]
        /* Столбец "(" */               {{LPAREN, EXPR, RPAREN, EXPR_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // ( <Выражение> ) <Выражение продолжение> | [][][][]
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{RBRACKET, ID, ASSIGN, LBRACE, INITIALIZERS, RBRACE, SEMICOL}, {SemanticActionType::ARRAY_INDEX, SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ASSIGN_OP}}, // ] идентификатор = { <Инициализаторы> } ; | i a[][][][] = 
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Альтернативное действие"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{ELSE, BLOCK}, {SemanticActionType::PROG_10, SemanticActionType::EMPTY_ACTION}}, // else <Блок> | 11[]
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{LAMBDA}, {}},
        /* Столбец "/" */               {{LAMBDA}, {}},
        /* Столбец "[" */               {{LAMBDA}, {}},
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "Блок"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{}, {}},
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{LBRACE, STMT_LIST, RBRACE}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // { <Список операторов> } | [][][]
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Присваивание продолжение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{}, {}},
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{LBRACKET, ARRAY_ASSIGN_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // [ <Присваивание массива продолжение> | [][]
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{ASSIGN, EXPR, SEMICOL}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ASSIGN_OP}}, // = <Выражение> ; | [][]=          
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Присваивание массива продолжение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, FACTOR_CONT, EXPR_CONT, RBRACKET, ASSIGN, EXPR, SEMICOL}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ARRAY_INDEX, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ASSIGN_OP}}, // идентификатор <Фактор продолжение> <Выражение продолжение>] = <Выражение>; | a[][]i[][] =        
        /* Столбец "константа" */       {{NUM, EXPR_CONT, RBRACKET, ASSIGN, EXPR, SEMICOL}, {SemanticActionType::PUSH_CONST, SemanticActionType::EMPTY_ACTION, SemanticActionType::ARRAY_INDEX, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ASSIGN_OP }}, // число <Выражение продолжение> ] = <Выражение> ; | k[] i[][] =      
        /* Столбец "(" */               {{LPAREN, EXPR, RPAREN, EXPR_CONT, RBRACKET, ASSIGN, EXPR, SEMICOL}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ARRAY_INDEX, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ASSIGN_OP}}, // ( <Выражение> ) <Выражение продолжение> ] = <Выражение> ; | [][][][] i[][] =         
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Ввод с консоли продолжение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, INPUT_ARRAY, SEMICOL}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::READ_OP}}, // идентификатор <Ввод массива> ; | а [][]
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Вывод в консоль продолжение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, OUTPUT_ARRAY, SEMICOL}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::WRITE_OP}}, // идентификатор <Вывод массива> ; | а [] w
        /* Столбец "константа" */       {{NUM, SEMICOL}, {SemanticActionType::PUSH_CONST, SemanticActionType::EMPTY_ACTION}}, // число ; | k []
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Проверка равенства"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{}, {}},
        /* Столбец "константа" */       {{}, {}},
        /* Столбец "(" */               {{}, {}},
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{EQ, EXPR, FOR_COMPARISON}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EQUAL_OP}}, // ==<Выражение><для_сравнений> | [][]==
        /* Столбец "!" */               {{NOT, EXPR, FOR_COMPARISON}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::NOT_OP}},   // !<Выражение><для_сравнений> | [][]!
        /* Столбец "<" */               {{LESS, EXPR, FOR_COMPARISON}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::LESS_OP}}, // < <Выражение><для_сравнений> | [][]<
        /* Столбец ">" */               {{GREATER, EXPR, FOR_COMPARISON}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::GREATER_OP}}, // > <Выражение><для_сравнений> | [][]>
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Выражение"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, FACTOR_CONT, EXPR_CONT}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Фактор продолжение> <Выражение продолжение>  | a[][]
        /* Столбец "константа" */       {{NUM, EXPR_CONT}, {SemanticActionType::PUSH_CONST, SemanticActionType::EMPTY_ACTION}}, // число <Выражение продолжение>  | k []
        /* Столбец "(" */               {{LPAREN, EXPR, RPAREN, EXPR_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION }}, // ( <Выражение> ) <Выражение продолжение>  | [][][][]
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Столбец "выражение продолжение"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{PLUS, TERM, EXPR_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ADD_OP}},  // +<Терм><Выражение продолжение> | [][]+
        /* Столбец "-" */               {{MINUS, TERM, EXPR_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::SUB_OP}}, // -<Терм><Выражение продолжение> | [][]-
        /* Столбец "*" */               {{MULT, TERM, EXPR_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::MUL_OP}},
        /* Столбец "/" */               {{DIV, TERM, EXPR_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::DIV_OP}},
        /* Столбец "[" */               {{LAMBDA}, {}},
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "Терм"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, FACTOR_CONT, TERM_CONT}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}},  // идентификатор <Фактор продожение><Терм продолжение> | a[][]
        /* Столбец "константа" */       {{NUM, TERM_CONT}, {SemanticActionType::PUSH_CONST, SemanticActionType::EMPTY_ACTION}}, // число <Терм продолжение> | k []
        /* Столбец "(" */               {{LPAREN, EXPR, RPAREN, TERM_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // ( <Выражение> ) <Терм продолжение>
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Терм продолжение"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{MULT, FACTOR, TERM_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::MUL_OP}}, // * <Фактор> <Терм продолжение> | [][]*
        /* Столбец "/" */               {{DIV, FACTOR, TERM_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::DIV_OP}}, // / <Фактор> <Терм продолжение> | [][]/
        /* Столбец "[" */               {{LAMBDA}, {}},
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "Фактор"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, FACTOR_CONT}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Фактор продолжение> | a[]
        /* Столбец "константа" */       {{NUM}, {SemanticActionType::PUSH_CONST}}, // число | k
        /* Столбец "(" */               {{LPAREN, EXPR, RPAREN}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // ( <Выражение> ) | [][][]
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "Фактор продолжение"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{LAMBDA}, {}},
        /* Столбец "/" */               {{LAMBDA}, {}},
        /* Столбец "[" */               {{LBRACKET, EXPR, RBRACKET}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ARRAY_INDEX}}, // [ <Выражение> ] | [][]i
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "инициализаторы"
    {
        /* Столбец "int" */             {{}, {}},
        /* Столбец "real" */            {{}, {}},
        /* Столбец "if" */              {{}, {}},
        /* Столбец "while" */           {{}, {}},
        /* Столбец "input" */           {{}, {}},
        /* Столбец "output" */          {{}, {}},
        /* Столбец "идентификатор" */   {{ID, FACTOR_CONT, INITIALIZERS_CONT}, {SemanticActionType::PUSH_ID, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // идентификатор <Фактор продолжение><Инициализаторы продолжение> | a[][]
        /* Столбец "константа" */       {{NUM, INITIALIZERS_CONT}, {SemanticActionType::PUSH_CONST, SemanticActionType::EMPTY_ACTION}}, // k[]
        /* Столбец "(" */               {{LPAREN, EXPR, RPAREN, INITIALIZERS_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, //( <Выражение> ) <Инициализаторы продолжение> || [][][][]
        /* Столбец "," */               {{}, {}},
        /* Столбец "else" */            {{}, {}},
        /* Столбец "{" */               {{}, {}},
        /* Столбец "==" */              {{}, {}},
        /* Столбец "!" */               {{}, {}},
        /* Столбец "<" */               {{}, {}},
        /* Столбец ">" */               {{}, {}},
        /* Столбец "+" */               {{}, {}},
        /* Столбец "-" */               {{}, {}},
        /* Столбец "*" */               {{}, {}},
        /* Столбец "/" */               {{}, {}},
        /* Столбец "[" */               {{}, {}},
        /* Столбец ";" */               {{}, {}},
        /* Столбец "=" */               {{}, {}},
        /* Столбец "]" */               {{}, {}},
        /* Столбец "}" */               {{}, {}},
        /* Столбец ")" */               {{}, {}},
    },

    // Строка "инициализаторы продолжение"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{COMMA, EXPR, INITIALIZERS_CONT}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION}}, // , <Выражение><Инициализаторы продолжение> | [][][]
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{LAMBDA}, {}},
        /* Столбец "/" */               {{LAMBDA}, {}},
        /* Столбец "[" */               {{LAMBDA}, {}},
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "для_сравнений"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{LAMBDA}, {}},
        /* Столбец "/" */               {{LAMBDA}, {}},
        /* Столбец "[" */               {{LAMBDA}, {}},
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "Вывод массива"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{LAMBDA}, {}},
        /* Столбец "/" */               {{LAMBDA}, {}},
        /* Столбец "[" */               {{LBRACKET, EXPR, RBRACKET}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ARRAY_INDEX}}, // [<Выражение>] | [][]i
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

    // Строка "Ввод массива"
    {
        /* Столбец "int" */             {{LAMBDA}, {}},
        /* Столбец "real" */            {{LAMBDA}, {}},
        /* Столбец "if" */              {{LAMBDA}, {}},
        /* Столбец "while" */           {{LAMBDA}, {}},
        /* Столбец "input" */           {{LAMBDA}, {}},
        /* Столбец "output" */          {{LAMBDA}, {}},
        /* Столбец "идентификатор" */   {{LAMBDA}, {}},
        /* Столбец "константа" */       {{LAMBDA}, {}},
        /* Столбец "(" */               {{LAMBDA}, {}},
        /* Столбец "," */               {{LAMBDA}, {}},
        /* Столбец "else" */            {{LAMBDA}, {}},
        /* Столбец "{" */               {{LAMBDA}, {}},
        /* Столбец "==" */              {{LAMBDA}, {}},
        /* Столбец "!" */               {{LAMBDA}, {}},
        /* Столбец "<" */               {{LAMBDA}, {}},
        /* Столбец ">" */               {{LAMBDA}, {}},
        /* Столбец "+" */               {{LAMBDA}, {}},
        /* Столбец "-" */               {{LAMBDA}, {}},
        /* Столбец "*" */               {{LAMBDA}, {}},
        /* Столбец "/" */               {{LAMBDA}, {}},
        /* Столбец "[" */               {{LBRACKET, EXPR, RBRACKET}, {SemanticActionType::EMPTY_ACTION, SemanticActionType::EMPTY_ACTION, SemanticActionType::ARRAY_INDEX}}, // [<Выражение>] | [][]i
        /* Столбец ";" */               {{LAMBDA}, {}},
        /* Столбец "=" */               {{LAMBDA}, {}},
        /* Столбец "]" */               {{LAMBDA}, {}},
        /* Столбец "}" */               {{LAMBDA}, {}},
        /* Столбец ")" */               {{LAMBDA}, {}},
    },

};




// Stack для Магазина с терминалами/нетерминалами
stack<int> magazine;
// Stack для Генератора ОПС
stack<SemanticActionType> forGenOPS;

// ОПС-типы для ОПС-элемента
enum class OPSTokenType {
    IDENTIFIER,
    CONST_INT,
    CONST_REAL,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE,
    OPERATOR_EQUALS,
    OPERATOR_LESS,
    OPERATOR_GREATER,
    OPERATOR_NOT, // !
    OPERATOR_ASSIGN,
    JUMP_FALSE, // jf
    JUMP, // j
    INDEX_ARRAY, //i
    READ, // r
    WRITE, // w
    GEN, // GEN
    LABEL, // метка
    LABEL_PLACEHOLDER, // пустое место для будущей метки ???
    INIT,
    LIST,
    UNKNOWN
};


// Структура ОПС-элемента
struct OPSToken {
    OPSTokenType type;
    string name;
    int metka;
    int intValue = 0;
    float realValue = 0.0f;

    OPSToken(OPSTokenType t, string n = "", int iVal = 0, float fVal = 0.0f)
        : type(t), name(n), metka(-1), intValue(iVal), realValue(fVal) {
    }
};


// Проверка на терминал
bool isTerminal(int symbol) {
    return symbol >= 1 && symbol <= 26;
}

// Проверка на нетерминал
bool isNonTerminal(int symbol) {
    return symbol >= 28 && symbol <= 50;
}


#include <map>
// --- ВЫПОЛНЕНИЕ СЕМАНТИЧЕСКИХ ДЕЙСТВИЙ ---
stack<string> Data_types;
stack<int> LabelStack;
map<string, int> VariableIntTable;
map<string, double> VariableRealTable;
int k = 0;
int m = 0;

// Массивы и паспорта
map<string, vector<int>> IntArrayTable;
map<string, vector<float>> RealArrayTable;

struct ArrayPassport {
    int baseAddress; // индекс первого элемента (0)
    int length;      // кол-во элементов
};

map<string, ArrayPassport> ArrayPassports;
map<string, bool> IsRealArray; // true — вещественный массив, false — целый

ArrayPassport currentArrayPassport;

void doSemanticAction(SemanticActionType action, int currentSymbol, const Token& token, vector<OPSToken>& opsResult);

void doSemanticAction(SemanticActionType action, int currentSymbol, const Token& token, vector<OPSToken>& opsResult) {
    switch (action) {
    case SemanticActionType::EMPTY_ACTION:
        break;


    case SemanticActionType::PUSH_ID:
        opsResult.emplace_back(OPSTokenType::IDENTIFIER, token.lexeme);
        k++;
        break;
    case SemanticActionType::PUSH_CONST:
        if (token.type == TokenType::Integer)
        {
            opsResult.emplace_back(OPSTokenType::CONST_INT, token.lexeme, token.intValue);
            k++;
        }
        else if (token.type == TokenType::Real)
        {
            opsResult.emplace_back(OPSTokenType::CONST_REAL, token.lexeme, 0, token.realValue);
            k++;
        }
        break;

        // Арифметические
    case SemanticActionType::ADD_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_PLUS, "+");
        k++;
        break;
    case SemanticActionType::SUB_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_MINUS, "-");
        k++;
        break;
    case SemanticActionType::MUL_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_MULTIPLY, "*");
        k++;
        break;
    case SemanticActionType::DIV_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_DIVIDE, "/");
        k++;
        break;

        // Сравнения и логика
    case SemanticActionType::EQUAL_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_EQUALS, "==");
        k++;
        break;
    case SemanticActionType::LESS_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_LESS, "<");
        k++;
        break;
    case SemanticActionType::GREATER_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_GREATER, ">");
        k++;
        break;
    case SemanticActionType::NOT_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_NOT, "!");
        k++;
        break;
    case SemanticActionType::ASSIGN_OP:
        opsResult.emplace_back(OPSTokenType::OPERATOR_ASSIGN, "=");
        k++;
        break;

        // Ввод/вывод
    case SemanticActionType::WRITE_OP:
        opsResult.emplace_back(OPSTokenType::WRITE, "w");
        k++;
        break;
    case SemanticActionType::READ_OP:
        opsResult.emplace_back(OPSTokenType::READ, "r");
        k++;
        break;

        // Массивы
    case SemanticActionType::ARRAY_INIT:
        break;
    case SemanticActionType::ARRAY_GEN:
        break;
    case SemanticActionType::ARRAY_INDEX:
        opsResult.emplace_back(OPSTokenType::INDEX_ARRAY, "<i>");
        k++;
        break;

        // Управление потоком
    case SemanticActionType::JUMP:
        opsResult.emplace_back(OPSTokenType::JUMP, "j");
        k++;
        break;
    case SemanticActionType::COND_JUMP_FALSE:
        opsResult.emplace_back(OPSTokenType::JUMP_FALSE, "jf");
        k++;
        break;

        // Программы
    case SemanticActionType::PROG_1:
        Data_types.push(token.lexeme);
        break;

    case SemanticActionType::PROG_2: {
        currentArrayPassport.baseAddress = 0;
        currentArrayPassport.length = token.intValue;
        break;
    }

    case SemanticActionType::PROG_3: {
        if (Data_types.empty())
            throw runtime_error("Data_types stack is empty");

        string type = Data_types.top();
        string varName = token.lexeme;

        if (VariableIntTable.count(varName) == 1)
            throw runtime_error("Повторное описание переменной: " + varName);

        if (VariableRealTable.count(varName) == 1)
            throw runtime_error("Повторное описание переменной: " + varName);

        if (type == "int")
        {
            VariableIntTable[varName] = 0;
        }
        else if (type == "real")
        {
            VariableRealTable[varName] = 0.0;
        }
        else {
            cout << "Error";
        }
        opsResult.emplace_back(OPSTokenType::IDENTIFIER, varName);
        k++;
        break;
    }

    case SemanticActionType::PROG_5:
        opsResult.emplace_back(OPSTokenType::INIT, "init");
        k++;
        opsResult.emplace_back(OPSTokenType::LIST, "$");
        k++;
        break;

    case SemanticActionType::PROG_6:    // в презентации программа 4
        LabelStack.push(k);
        break;

    case SemanticActionType::PROG_7:    // в презентации программа 1
        LabelStack.push(k);
        opsResult.emplace_back(OPSTokenType::LABEL_PLACEHOLDER, "");
        k++;
        opsResult.emplace_back(OPSTokenType::JUMP_FALSE, "jf");
        k++;
        break;

    case SemanticActionType::PROG_8:    // в презентации программа 5 
    {
        if (LabelStack.empty())
            throw runtime_error("Label stack is empty");

        int label = LabelStack.top(); LabelStack.pop();
        opsResult[label] = OPSToken(OPSTokenType::LABEL, "L" + to_string(k + 2));
        label = LabelStack.top(); LabelStack.pop();
        opsResult.emplace_back(OPSTokenType::LABEL, "L" + to_string(label));
        k++;
        opsResult.emplace_back(OPSTokenType::JUMP, "j");
        k++;
        break;
    }

    case SemanticActionType::PROG_9:    // в презентации программа 1
        LabelStack.push(k);
        opsResult.emplace_back(OPSTokenType::LABEL_PLACEHOLDER, "");
        k++;
        opsResult.emplace_back(OPSTokenType::JUMP_FALSE, "jf");
        k++;
        break;

    case SemanticActionType::PROG_10:   // в презентации программа 2 
    {
        if (LabelStack.empty())
            throw runtime_error("Label stack is empty");

        int oldLabel = LabelStack.top(); LabelStack.pop();
        opsResult[oldLabel] = OPSToken(OPSTokenType::LABEL, "L" + to_string(k + 2));
        LabelStack.push(k);
        opsResult.emplace_back(OPSTokenType::LABEL_PLACEHOLDER, "");
        k++;
        opsResult.emplace_back(OPSTokenType::JUMP, "j");
        k++;
        break;
    }

    case SemanticActionType::PROG_11:   // в презентации программа 3 
    {
        if (LabelStack.empty())
            throw runtime_error("Label stack is empty");


        int label = LabelStack.top(); LabelStack.pop();
        opsResult[label] = OPSToken(OPSTokenType::LABEL, "L" + to_string(k));
        break;
    }

    case SemanticActionType::PROG_12: {
        ArrayPassports[token.lexeme] = currentArrayPassport;
        // cout << Data_types.top() << endl;
        if (Data_types.top() == "int")
        {
            IntArrayTable[token.lexeme] = vector<int>(currentArrayPassport.length);;
        }
        else if (Data_types.top() == "real")
        {
            RealArrayTable[token.lexeme] = vector<float>(currentArrayPassport.length);
        }
        else
        {
            throw runtime_error("Неизвестный тип данных при объявлении массива!");
        }
        break;
    }

    default:
        break;
    }
}



// --- ОТЛАДКА ДЛЯ СЕМАНТИЧЕСКОГО АНАЛИЗАТОРА И ГЕНЕРАТОРА ОПС ---
// Отображение символьного значения терминала/нетерминала по номеру 
string symbolToString(int symbol) {
    switch (symbol) {
        // Терминалы
    case INT: return "int";
    case REAL: return "real";
    case IF: return "if";
    case WHILE: return "while";
    case INPUT: return "input";
    case OUTPUT: return "output";
    case ID: return "id";
    case NUM: return "num";
    case LPAREN: return "(";
    case COMMA: return ",";
    case ELSE: return "else";
    case LBRACE: return "{";
    case EQ: return "==";
    case NOT: return "!";
    case LESS: return "<";
    case GREATER: return ">";
    case PLUS: return "+";
    case MINUS: return "-";
    case MULT: return "*";
    case DIV: return "/";
    case LBRACKET: return "[";
    case SEMICOL: return ";";
    case ASSIGN: return "=";
    case RBRACKET: return "]";
    case RBRACE: return "}";
    case RPAREN: return ")";

        // Нетерминалы
    case PROGRAM: return "<Программа>";
    case VAR_DECL: return "<Объявление переменной>";
    case STMT_LIST: return "<Список операторов>";
    case VAR_DECL_CONT: return "<Объявление продолжение>";
    case ARRAY_DECL_CONT: return "<Объявление массива продолжение>";
    case ALT_ACTION: return "<Альтернативное действие>";
    case BLOCK: return "<Блок>";
    case ASSIGN_CONT: return "<Присваивание продолжение>";
    case ARRAY_ASSIGN_CONT: return "<Присваивание массива продолжение>";
    case INPUT_CONT: return "<Ввод с консоли продолжение>";
    case OUTPUT_CONT: return "<Вывод в консоль продолжение>";
    case COMPARISON: return "<Проверка равенства>";
    case EXPR: return "<Выражение>";
    case EXPR_CONT: return "<Выражение продолжение>";
    case TERM: return "<Терм>";
    case TERM_CONT: return "<Терм продолжение>";
    case FACTOR: return "<Фактор>";
    case FACTOR_CONT: return "<Фактор продолжение>";
    case INITIALIZERS: return "<Инициализаторы>";
    case INITIALIZERS_CONT: return "<Инициализаторы продолжение>";
    case FOR_COMPARISON: return "<Для_сравнений>";
    case OUTPUT_ARRAY: return "<Вывод массива>";
    case INPUT_ARRAY: return "<Ввод массива>";


        // Специальные символы
    case LAMBDA: return "λ";
    case NONE: return "<NONE>";
    case EOF: return "<EOF>";

        // По умолчанию
    default: return "Symbol" + to_string(symbol);
    }
}


// Вывод содержимого стека magazine в строку
string stackToString(stack<int> magazine) {
    ostringstream oss;
    vector<int> temp;

    // Переносим в вектор для правильного порядка
    while (!magazine.empty()) {
        temp.push_back(magazine.top());
        magazine.pop();
    }

    // Выводим от верхушки до дна
    for (auto it = temp.begin(); it != temp.end(); ++it) {
        oss << symbolToString(*it) << " ";
    }

    return oss.str();
}

// Вывод содержимого стека forGenOPS в строку
string semanticStackToString(stack<SemanticActionType> forGenOPS) {
    ostringstream oss;
    vector<SemanticActionType> temp;

    while (!forGenOPS.empty()) {
        temp.push_back(forGenOPS.top());
        forGenOPS.pop();
    }

    for (auto it = temp.begin(); it != temp.end(); ++it) {
        switch (*it) {
        case SemanticActionType::EMPTY_ACTION:    oss << "EMPTY "; break;
        case SemanticActionType::PUSH_ID:         oss << "PUSH_ID "; break;
        case SemanticActionType::PUSH_CONST:      oss << "PUSH_CONST "; break;
        case SemanticActionType::WRITE_OP:        oss << "WRITE "; break;
        case SemanticActionType::READ_OP:         oss << "READ "; break;
        case SemanticActionType::ARRAY_INIT:      oss << "ARRAY_INIT "; break;
        case SemanticActionType::ARRAY_GEN:       oss << "ARRAY_GEN "; break;
        case SemanticActionType::JUMP:            oss << "JUMP "; break;
        case SemanticActionType::COND_JUMP_FALSE: oss << "JF "; break;
        case SemanticActionType::ARRAY_INDEX:     oss << "ARRAY_INDEX "; break;

        case SemanticActionType::ADD_OP:          oss << "ADD "; break;
        case SemanticActionType::SUB_OP:          oss << "SUB "; break;
        case SemanticActionType::MUL_OP:          oss << "MUL "; break;
        case SemanticActionType::DIV_OP:          oss << "DIV "; break;

        case SemanticActionType::LESS_OP:         oss << "LESS "; break;
        case SemanticActionType::GREATER_OP:      oss << "GREATER "; break;
        case SemanticActionType::NOT_OP:          oss << "NOT "; break;
        case SemanticActionType::EQUAL_OP:        oss << "EQUAL "; break;
        case SemanticActionType::ASSIGN_OP:       oss << "ASSIGN "; break;

        case SemanticActionType::PROG_1:          oss << "PROG_1 "; break;
        case SemanticActionType::PROG_2:          oss << "PROG_2 "; break;
        case SemanticActionType::PROG_3:          oss << "PROG_3 "; break;
        case SemanticActionType::PROG_4:          oss << "PROG_4 "; break;
        case SemanticActionType::PROG_5:          oss << "PROG_5 "; break;
        case SemanticActionType::PROG_6:          oss << "PROG_6 "; break;
        case SemanticActionType::PROG_7:          oss << "PROG_7 "; break;
        case SemanticActionType::PROG_8:          oss << "PROG_8 "; break;
        case SemanticActionType::PROG_9:          oss << "PROG_9 "; break;
        case SemanticActionType::PROG_10:         oss << "PROG_10 "; break;
        case SemanticActionType::PROG_11:         oss << "PROG_11 "; break;
        case SemanticActionType::PROG_12:         oss << "PROG_12 "; break;

        default: oss << "??? "; break;
        }
    }

    return oss.str();
}


// --- ФУНКЦИЯ СЕМАНТИЧЕСКОГО АНАЛИЗАТОРА И ГЕНЕРАТОРА ОПС ---
void semanticAnalyzerAndGenOPS(const vector<Token>& tokens);

vector<OPSToken> opsResult;

void semanticAnalyzerAndGenOPS(const vector<Token>& tokens) {
    stack<int> magazine;
    stack<SemanticActionType> forGenOPS;

    int tokenIndex = 0;
    Token currentToken = tokens[tokenIndex];

    magazine.push(PROGRAM); // PROGRAM = 28, как указано
    forGenOPS.push(SemanticActionType::EMPTY_ACTION);

    while (!magazine.empty()) {
        // cout << "Текущий магазин:      " << stackToString(magazine) << endl;
        // cout << "Текущие сем.действия: " << semanticStackToString(forGenOPS) << endl;


        int currentSymbol = magazine.top(); magazine.pop();
        SemanticActionType currentAction = forGenOPS.top(); forGenOPS.pop();

        currentToken = tokens[tokenIndex];

        // cout << "Текущая лексема: " << currentToken.lexeme << endl << endl;

        // Выполнить семантическое действие
        doSemanticAction(currentAction, currentSymbol, currentToken, opsResult);

        if (currentToken.numLexeme == -2)
        {
            break;
        }


        // Если это терминал
        if (isTerminal(currentSymbol)) {

            if (currentSymbol == currentToken.numLexeme) {
                tokenIndex++;
            }
            else {
                std::cerr << "Синтаксическая ошибка: ожидался терминал " << currentSymbol
                    << ", но получен " << currentToken.numLexeme << " " << currentToken.lexeme << " "
                    << " (строка " << currentToken.line << " позиция " << currentToken.currLinePos << ")\n";
                return;
            }
        }
        // Если это нетерминал
        else if (isNonTerminal(currentSymbol)) {

            Rule rule = semanticTable[currentSymbol - 28][currentToken.numLexeme - 1];

            if (rule.rightRule.empty()) {
                // Правило = λ, ничего не добавляем
                cout << "Синтаксическая ошибка!" << endl;
                cout << currentSymbol << endl;
                cout << currentToken.numLexeme << endl;
                return;
            }
            else if (rule.rightRule[0] == LAMBDA) {
                // Правило = λ, ничего не добавляем
                continue;
            }

            // Добавляем правило справа налево
            for (int i = static_cast<int>(rule.rightRule.size()) - 1; i >= 0; --i) {
                magazine.push(rule.rightRule[i]);
                forGenOPS.push(rule.semanticActions[i]);
            }
        }

    }

    if (magazine.empty() && (tokenIndex < tokens.size()))
    {
        if (tokens[tokenIndex].numLexeme != -2)
        {
            cout << "Семантическая ошибка! В строке номер " << tokens[tokenIndex].line
                << " позиция " << tokens[tokenIndex].currLinePos
                << endl;
            return;
        }
    }

    VariableRealTable.erase("real");

    // Вывод результата
    //cout << "Сгенерированная ОПС:\n";
    //for (const auto& token : opsResult) {
    //    cout << "Тип: " << static_cast<int>(token.type)
    //        << ", Имя: " << token.name
    //        << ", int: " << token.intValue
    //        << ", float: " << token.realValue
    //        << ", метка: " << token.metka << '\n';
    //}
}
// ******************************* SEMANTIC ANALIZATOR *******************************




// ******************************* OPS INTERPRETER *******************************
#include <stack>
#include <queue>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

enum class OperandValueType { INTEGER_VALUE, REAL_VALUE };

struct OperandValue {
    OperandValueType type;
    int intValue;
    double realValue;

    OperandValue(int v) : type(OperandValueType::INTEGER_VALUE), intValue(v), realValue(static_cast<double>(v)) {}
    OperandValue(float v) : type(OperandValueType::REAL_VALUE), intValue(static_cast<int>(v)), realValue(v) {}
};

enum class ContentTypes
{
    IntVar,
    RealVar,

    IntConst,
    RealConst,

    IntResult,
    RealResult,

    IntArrayEl,
    RealArrayEl,

    BooleanResult,
    Label
};

struct elMagazine {
    ContentTypes contentType;

    string linkVariable; // ссылка на переменную

    int intConst; // константа целочисленная
    double realConst; // константа вещественная

    int intResult; // целочисленный результат
    double realResult; // вещественный результат

    string linkArrayPassport; // ссылка на паспорт массива

    int linkArrayElement; // ссылка на элемент массива

    bool booleanResult;
    int label;
};

void printElMagazine(const elMagazine& el) {
    cout << "elMagazine {\n";

    switch (el.contentType) {
    case ContentTypes::IntVar:
        cout << "  contentType: IntVar\n";
        cout << "  linkVariable: " << el.linkVariable << "\n";
        break;

    case ContentTypes::RealVar:
        cout << "  contentType: RealVar\n";
        cout << "  linkVariable: " << el.linkVariable << "\n";
        break;

    case ContentTypes::IntConst:
        cout << "  contentType: IntConst\n";
        cout << "  intConst: " << el.intConst << "\n";
        break;

    case ContentTypes::RealConst:
        cout << "  contentType: RealConst\n";
        cout << "  realConst: " << el.realConst << "\n";
        break;

    case ContentTypes::IntResult:
        cout << "  contentType: IntResult\n";
        cout << "  intResult: " << el.intResult << "\n";
        break;

    case ContentTypes::RealResult:
        cout << "  contentType: RealResult\n";
        cout << "  realResult: " << el.realResult << "\n";
        break;

    case ContentTypes::IntArrayEl:
        cout << "  contentType: IntArrayEl\n";
        cout << "  linkArrayPassport: " << el.linkArrayPassport << "\n";
        cout << "  linkArrayElement: " << el.linkArrayElement << "\n";
        break;

    case ContentTypes::RealArrayEl:
        cout << "  contentType: RealArrayEl\n";
        cout << "  linkArrayPassport: " << el.linkArrayPassport << "\n";
        cout << "  linkArrayElement: " << el.linkArrayElement << "\n";
        break;

    default:
        cout << "  contentType: UNKNOWN\n";
        break;
    }

    cout << "}\n";
}

void printMagazineStack(stack<elMagazine> original) {
    cout << "=== Содержимое стека ===" << endl;

    while (!original.empty()) {
        const elMagazine& el = original.top();

        printElMagazine(el);

        original.pop();  // идём к следующему
    }

    cout << "=========================" << endl;
}


void opsInterpreter()
{
    stack<elMagazine> interpreterMagazine; // магазин интерпретатора

    for (size_t i = 0; i < opsResult.size(); ++i) {
        const auto& currentToken = opsResult[i];

        switch (currentToken.type)
        {
        case OPSTokenType::CONST_INT:
        {
            elMagazine currentElMagazine;
            currentElMagazine.intConst = currentToken.intValue;
            currentElMagazine.contentType = ContentTypes::IntConst;
            interpreterMagazine.push(currentElMagazine);
            break;
        }
        case OPSTokenType::CONST_REAL:
        {
            elMagazine currentElMagazine;
            currentElMagazine.realConst = currentToken.realValue;
            currentElMagazine.contentType = ContentTypes::RealConst;
            interpreterMagazine.push(currentElMagazine);
            break;
        }
        case OPSTokenType::IDENTIFIER:
        {
            if (VariableIntTable.count(currentToken.name))
            {
                elMagazine currentElMagazine;
                currentElMagazine.linkVariable = currentToken.name;
                currentElMagazine.contentType = ContentTypes::IntVar;
                interpreterMagazine.push(currentElMagazine);
            }
            else if (VariableRealTable.count(currentToken.name))
            {
                elMagazine currentElMagazine;
                currentElMagazine.linkVariable = currentToken.name;
                currentElMagazine.contentType = ContentTypes::RealVar;
                interpreterMagazine.push(currentElMagazine);
            }
            else if (IntArrayTable.count(currentToken.name))
            {
                elMagazine currentElMagazine;
                currentElMagazine.linkArrayPassport = currentToken.name;
                currentElMagazine.contentType = ContentTypes::IntArrayEl;
                currentElMagazine.linkArrayElement = ArrayPassports[currentToken.name].baseAddress;
                interpreterMagazine.push(currentElMagazine);
            }
            else if (RealArrayTable.count(currentToken.name))
            {
                elMagazine currentElMagazine;
                currentElMagazine.linkArrayPassport = currentToken.name;
                currentElMagazine.contentType = ContentTypes::RealArrayEl;
                currentElMagazine.linkArrayElement = ArrayPassports[currentToken.name].baseAddress;
                interpreterMagazine.push(currentElMagazine);
            }
            else {
                throw runtime_error("Переменная не найдена! Ошибка.");
                return;
            }
            break;
        }
        case OPSTokenType::OPERATOR_PLUS:
        case OPSTokenType::OPERATOR_MINUS:
        case OPSTokenType::OPERATOR_MULTIPLY:
        case OPSTokenType::OPERATOR_DIVIDE:
        {
            if (interpreterMagazine.size() < 2) {
                cerr << "Ошибка: недостаточно операндов для арифметической операции\n";
                break;
            }

            elMagazine rightOperand = interpreterMagazine.top(); interpreterMagazine.pop();
            elMagazine leftOperand = interpreterMagazine.top(); interpreterMagazine.pop();

            double valA = 0.0, valB = 0.0;
            bool isIntA = false, isIntB = false;

            // Определяем значение правого операнда
            switch (rightOperand.contentType) {
            case ContentTypes::IntConst: valA = rightOperand.intConst; isIntA = true; break;
            case ContentTypes::RealConst: valA = rightOperand.realConst; break;
            case ContentTypes::IntResult: valA = rightOperand.intResult; isIntA = true; break;
            case ContentTypes::RealResult: valA = rightOperand.realResult; break;
            case ContentTypes::IntVar: valA = VariableIntTable[rightOperand.linkVariable]; isIntA = true; break;
            case ContentTypes::RealVar: valA = VariableRealTable[rightOperand.linkVariable]; break;
            case ContentTypes::IntArrayEl: valA = IntArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement]; isIntA = true; break;
            case ContentTypes::RealArrayEl: valA = RealArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement]; break;
            default: throw runtime_error("Неизвестный тип правого операнда");
            }

            // Определяем значение левого операнда
            switch (leftOperand.contentType) {
            case ContentTypes::IntConst: valB = leftOperand.intConst; isIntB = true; break;
            case ContentTypes::RealConst: valB = leftOperand.realConst; break;
            case ContentTypes::IntResult: valB = leftOperand.intResult; isIntB = true; break;
            case ContentTypes::RealResult: valB = leftOperand.realResult; break;
            case ContentTypes::IntVar: valB = VariableIntTable[leftOperand.linkVariable]; isIntB = true; break;
            case ContentTypes::RealVar: valB = VariableRealTable[leftOperand.linkVariable]; break;
            case ContentTypes::IntArrayEl: valB = IntArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement]; isIntB = true; break;
            case ContentTypes::RealArrayEl: valB = RealArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement]; break;
            default: throw runtime_error("Неизвестный тип левого операнда");
            }

            double result = 0.0;

            switch (currentToken.type) {
            case OPSTokenType::OPERATOR_PLUS:     result = valB + valA; break;
            case OPSTokenType::OPERATOR_MINUS:    result = valB - valA; break;
            case OPSTokenType::OPERATOR_MULTIPLY: result = valB * valA; break;
            case OPSTokenType::OPERATOR_DIVIDE:
                if (valA == 0.0) {
                    cerr << "Ошибка: деление на ноль\n";
                    continue;
                }
                result = valB / valA;
                break;
            default: break;
            }

            elMagazine resultEl;

            // если оба были целые и результат целый — сохранить как intResult
            if (isIntA && isIntB && floor(result) == result) {
                resultEl.contentType = ContentTypes::IntResult;
                resultEl.intResult = static_cast<int>(result);
            }
            else {
                resultEl.contentType = ContentTypes::RealResult;
                resultEl.realResult = result;
            }

            interpreterMagazine.push(resultEl);

            //cout << "Операция: " << valB << " "
            //    << ((currentToken.type == OPSTokenType::OPERATOR_PLUS) ? "+" :
            //        (currentToken.type == OPSTokenType::OPERATOR_MINUS) ? "-" :
            //        (currentToken.type == OPSTokenType::OPERATOR_MULTIPLY) ? "*" : "/")
            //    << " " << valA << " = " << result << "\n";

            break;
        }
        case OPSTokenType::INDEX_ARRAY:
        {
            if (interpreterMagazine.size() < 2) {
                cerr << "Ошибка: недостаточно операндов для операции индексирования массива\n";
                break;
            }

            elMagazine arrElIndex = interpreterMagazine.top(); interpreterMagazine.pop();
            elMagazine arrPassport = interpreterMagazine.top(); interpreterMagazine.pop();

            if (arrPassport.contentType == ContentTypes::IntArrayEl)
            {
                elMagazine currentElMagazine;
                currentElMagazine.contentType = ContentTypes::IntArrayEl;
                currentElMagazine.linkArrayPassport = arrPassport.linkArrayPassport;

                switch (arrElIndex.contentType)
                {
                case ContentTypes::IntVar:
                {
                    currentElMagazine.linkArrayElement = VariableIntTable[arrElIndex.linkVariable];
                    break;
                }
                case ContentTypes::IntArrayEl:
                {
                    currentElMagazine.linkArrayElement = IntArrayTable[arrElIndex.linkArrayPassport][arrElIndex.linkArrayElement];
                    break;
                }
                case ContentTypes::IntConst:
                {
                    currentElMagazine.linkArrayElement = arrElIndex.intConst;
                    break;
                }
                case ContentTypes::IntResult:
                {
                    currentElMagazine.linkArrayElement = arrElIndex.intResult;
                    break;
                }
                default:
                    currentElMagazine.linkArrayElement = 0;
                    break;
                }

                interpreterMagazine.push(currentElMagazine);
            }
            else if (arrPassport.contentType == ContentTypes::RealArrayEl)
            {
                elMagazine currentElMagazine;
                currentElMagazine.contentType = ContentTypes::RealArrayEl;
                currentElMagazine.linkArrayPassport = arrPassport.linkArrayPassport;

                switch (arrElIndex.contentType)
                {
                case ContentTypes::IntVar:
                {
                    currentElMagazine.linkArrayElement = VariableIntTable[arrElIndex.linkVariable];
                    break;
                }
                case ContentTypes::IntArrayEl:
                {
                    currentElMagazine.linkArrayElement = IntArrayTable[arrElIndex.linkArrayPassport][arrElIndex.linkArrayElement];
                    break;
                }
                case ContentTypes::IntConst:
                {
                    currentElMagazine.linkArrayElement = arrElIndex.intConst;
                    break;
                }
                case ContentTypes::IntResult:
                {
                    currentElMagazine.linkArrayElement = arrElIndex.intResult;
                    break;
                }
                default:
                    currentElMagazine.linkArrayElement = 0;
                    break;
                }

                interpreterMagazine.push(currentElMagazine);
            }
            else
            {
                throw runtime_error("Ошибка!");
            }

            break;
        }
        case OPSTokenType::OPERATOR_ASSIGN:
        {
            if (interpreterMagazine.size() < 2) {
                cerr << "Ошибка: недостаточно операндов для операции присваивания\n";
                break;
            }

            elMagazine rightOperand = interpreterMagazine.top(); interpreterMagazine.pop();
            elMagazine leftOperand = interpreterMagazine.top(); interpreterMagazine.pop();

            if (leftOperand.contentType == ContentTypes::IntVar) // левый операнд - переменная int
            {
                switch (rightOperand.contentType)
                {
                case ContentTypes::IntVar:  // правый операнд - переменная int
                {
                    VariableIntTable[leftOperand.linkVariable] = VariableIntTable[rightOperand.linkVariable];
                    break;
                }
                case ContentTypes::IntConst:    // правый операнд - константа int
                {
                    VariableIntTable[leftOperand.linkVariable] = rightOperand.intConst;
                    break;
                }
                case ContentTypes::IntResult:   // правый операнд - результат вычислений int
                {
                    VariableIntTable[leftOperand.linkVariable] = rightOperand.intResult;
                    break;
                }
                case ContentTypes::IntArrayEl:  // правый операнд - элемент int массива
                {
                    VariableIntTable[leftOperand.linkVariable] = IntArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement];
                    break;
                }
                default:
                    throw runtime_error("Ошибка!");
                    break;
                }
            }
            else if (leftOperand.contentType == ContentTypes::RealVar) // левый операнд - переменная real
            {
                switch (rightOperand.contentType)
                {
                case ContentTypes::RealVar:  // правый операнд - переменная real
                {
                    VariableRealTable[leftOperand.linkVariable] = VariableRealTable[rightOperand.linkVariable];
                    break;
                }
                case ContentTypes::RealConst:    // правый операнд - константа real
                {
                    VariableRealTable[leftOperand.linkVariable] = rightOperand.realConst;
                    break;
                }
                case ContentTypes::RealResult:   // правый операнд - результат вычислений real
                {
                    VariableRealTable[leftOperand.linkVariable] = rightOperand.realResult;
                    break;
                }
                case ContentTypes::RealArrayEl:  // правый операнд - элемент real массива
                {
                    VariableRealTable[leftOperand.linkVariable] = RealArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement];
                    break;
                }
                default:
                    throw runtime_error("Ошибка!");
                    break;
                }
            }
            else if (leftOperand.contentType == ContentTypes::IntArrayEl) // левый операнд - элемент int массива
            {
                switch (rightOperand.contentType)
                {
                case ContentTypes::IntVar:  // правый операнд - переменная int
                {
                    IntArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = VariableIntTable[rightOperand.linkVariable];
                    break;
                }
                case ContentTypes::IntConst:    // правый операнд - константа int
                {
                    IntArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = rightOperand.intConst;
                    break;
                }
                case ContentTypes::IntResult:   // правый операнд - результат вычислений int
                {
                    IntArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = rightOperand.intResult;
                    break;
                }
                case ContentTypes::IntArrayEl:  // правый операнд - элемент int массива
                {
                    IntArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = IntArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement];
                    break;
                }
                default:
                    throw runtime_error("Ошибка!");
                    break;
                }
            }
            else if (leftOperand.contentType == ContentTypes::RealArrayEl) // левый операнд - элемент real массива
            {
                switch (rightOperand.contentType)
                {
                case ContentTypes::RealVar:  // правый операнд - переменная real
                {
                    RealArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = VariableRealTable[rightOperand.linkVariable];
                    break;
                }
                case ContentTypes::RealConst:    // правый операнд - константа real
                {
                    RealArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = rightOperand.realConst;
                    break;
                }
                case ContentTypes::RealResult:   // правый операнд - результат вычислений real
                {
                    RealArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = rightOperand.realResult;
                    break;
                }
                case ContentTypes::RealArrayEl:  // правый операнд - элемент real массива
                {
                    RealArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement] = RealArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement];
                    break;
                }
                default:
                    throw runtime_error("Ошибка!");
                    break;
                }
            }
            else
            {
                throw runtime_error("Ошибка!");
            }
            break;
        }
        case OPSTokenType::OPERATOR_NOT:
        case OPSTokenType::OPERATOR_EQUALS:
        case OPSTokenType::OPERATOR_GREATER:
        case OPSTokenType::OPERATOR_LESS:
        {
            if (interpreterMagazine.size() < 2) {
                cerr << "Ошибка: недостаточно операндов для логической операции\n";
                break;
            }

            elMagazine rightOperand = interpreterMagazine.top(); interpreterMagazine.pop();
            elMagazine leftOperand = interpreterMagazine.top(); interpreterMagazine.pop();

            double valA = 0.0, valB = 0.0;
            bool isIntA = false, isIntB = false;

            // Определяем значение правого операнда
            switch (rightOperand.contentType) {
            case ContentTypes::IntConst: valA = rightOperand.intConst; isIntA = true; break;
            case ContentTypes::RealConst: valA = rightOperand.realConst; break;
            case ContentTypes::IntResult: valA = rightOperand.intResult; isIntA = true; break;
            case ContentTypes::RealResult: valA = rightOperand.realResult; break;
            case ContentTypes::IntVar: valA = VariableIntTable[rightOperand.linkVariable]; isIntA = true; break;
            case ContentTypes::RealVar: valA = VariableRealTable[rightOperand.linkVariable]; break;
            case ContentTypes::IntArrayEl: valA = IntArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement]; isIntA = true; break;
            case ContentTypes::RealArrayEl: valA = RealArrayTable[rightOperand.linkArrayPassport][rightOperand.linkArrayElement]; break;
            default: throw runtime_error("Неизвестный тип правого операнда");
            }

            // Определяем значение левого операнда
            switch (leftOperand.contentType) {
            case ContentTypes::IntConst: valB = leftOperand.intConst; isIntB = true; break;
            case ContentTypes::RealConst: valB = leftOperand.realConst; break;
            case ContentTypes::IntResult: valB = leftOperand.intResult; isIntB = true; break;
            case ContentTypes::RealResult: valB = leftOperand.realResult; break;
            case ContentTypes::IntVar: valB = VariableIntTable[leftOperand.linkVariable]; isIntB = true; break;
            case ContentTypes::RealVar: valB = VariableRealTable[leftOperand.linkVariable]; break;
            case ContentTypes::IntArrayEl: valB = IntArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement]; isIntB = true; break;
            case ContentTypes::RealArrayEl: valB = RealArrayTable[leftOperand.linkArrayPassport][leftOperand.linkArrayElement]; break;
            default: throw runtime_error("Неизвестный тип левого операнда");
            }

            bool result = false;

            switch (currentToken.type) {
            case OPSTokenType::OPERATOR_NOT:     result = (valB != valA); break;
            case OPSTokenType::OPERATOR_EQUALS:    result = (valB == valA); break;
            case OPSTokenType::OPERATOR_GREATER: result = (valB > valA); break;
            case OPSTokenType::OPERATOR_LESS:   result = (valB < valA); break;
            default: break;
            }

            elMagazine resultEl;

            resultEl.contentType = ContentTypes::BooleanResult;
            resultEl.booleanResult = result;

            interpreterMagazine.push(resultEl);

            //cout << "Операция: " << valB << " "
            //    << ((currentToken.type == OPSTokenType::OPERATOR_NOT) ? "!" :
            //        (currentToken.type == OPSTokenType::OPERATOR_EQUALS) ? "==" :
            //        (currentToken.type == OPSTokenType::OPERATOR_GREATER) ? ">" : "<")
            //    << " " << valA << " = " << result << "\n";

            break;
        }
        case OPSTokenType::LABEL:
        {
            elMagazine currentElMagazine;
            currentElMagazine.contentType = ContentTypes::Label;

            // Извлекаем числовую часть из строки, удаляя 'L' в начале
            if (!currentToken.name.empty() && currentToken.name[0] == 'L')
            {
                string numberPart = currentToken.name.substr(1); // убираем 'L'
                try
                {
                    currentElMagazine.label = stoi(numberPart);
                }
                catch (const invalid_argument&)
                {
                    throw runtime_error("Некорректный формат метки: " + currentToken.name);
                }
                catch (const out_of_range&)
                {
                    throw runtime_error("Слишком большое значение метки: " + currentToken.name);
                }
            }
            else
            {
                throw runtime_error("Метка не начинается с 'L': " + currentToken.name);
            }

            interpreterMagazine.push(currentElMagazine);
            break;
        }
        case OPSTokenType::JUMP:
        {
            elMagazine labelOperand = interpreterMagazine.top(); interpreterMagazine.pop();
            i = labelOperand.label - 1;
            continue;
        }
        case OPSTokenType::JUMP_FALSE:
        {
            if (interpreterMagazine.size() < 2) {
                cerr << "Ошибка: недостаточно операндов для операции jump_false\n";
                break;
            }


            elMagazine labelOperand = interpreterMagazine.top(); interpreterMagazine.pop();
            elMagazine booleanResultOperand = interpreterMagazine.top(); interpreterMagazine.pop();

            if (booleanResultOperand.booleanResult == false)
            {
                i = labelOperand.label - 1;
                // cout << "Переход по метке " << labelOperand.label << " совершён, т.к. false." << endl;
                continue;
            }
            else
            {
                // cout << "Переход по метке пропущен, т.к. true." << endl;
            }
            break;
        }
        case OPSTokenType::READ:
        {
            if (interpreterMagazine.size() == 0) {
                cerr << "Ошибка: недостаточно операндов для операции чтения\n";
                break;
            }

            elMagazine operandForReading = interpreterMagazine.top(); interpreterMagazine.pop();

            if (operandForReading.contentType == ContentTypes::IntVar)
            {
                cout << "[READ] Введите значение для целочисленной переменной '" << operandForReading.linkVariable << "': ";
                string input;
                cin >> input;
                try
                {
                    int val = stoi(input);
                    VariableIntTable[operandForReading.linkVariable] = val;
                    // cout << "  → сохранено как целое: " << val << "\n";
                }
                catch (const exception& e)
                {
                    cerr << "Ошибка при чтении значения: " << e.what() << "\n";
                }
            }
            else if (operandForReading.contentType == ContentTypes::RealVar)
            {
                cout << "[READ] Введите значение для вещественной переменной '" << operandForReading.linkVariable << "': ";
                string input;
                cin >> input;
                replace(input.begin(), input.end(), '.', ',');
                try
                {
                    if (input.find(',') != string::npos)
                    {
                        float val = stof(input);
                        VariableRealTable[operandForReading.linkVariable] = static_cast<double>(val);
                        // cout << "  → сохранено как вещественное: " << val << "\n";
                    }
                }
                catch (const exception& e)
                {
                    cerr << "Ошибка при чтении значения: " << e.what() << "\n";
                }
            }
            else if (operandForReading.contentType == ContentTypes::IntArrayEl)
            {
                cout << "[READ] Введите значение для целочисленного элемента массива '" << operandForReading.linkArrayPassport << "': ";
                string input;
                cin >> input;
                try
                {
                    int val = stoi(input);
                    IntArrayTable[operandForReading.linkArrayPassport][operandForReading.linkArrayElement] = val;
                    // cout << "  → сохранено как целое: " << val << "\n";
                }
                catch (const exception& e)
                {
                    cerr << "Ошибка при чтении значения: " << e.what() << "\n";
                }
            }
            else if (operandForReading.contentType == ContentTypes::RealArrayEl)
            {
                cout << "[READ] Введите значение для вещественного элемента массива '" << operandForReading.linkArrayPassport << "': ";
                string input;
                cin >> input;
                replace(input.begin(), input.end(), '.', ',');
                try
                {
                    if (input.find(',') != string::npos)
                    {
                        float val = stof(input);
                        RealArrayTable[operandForReading.linkArrayPassport][operandForReading.linkArrayElement] = static_cast<double>(val);
                        // cout << "  → сохранено как вещественное: " << val << "\n";
                    }
                }
                catch (const exception& e)
                {
                    cerr << "Ошибка при чтении значения: " << e.what() << "\n";
                }
            }
            else
            {
                throw runtime_error("Операнд не предназначен для чтения!\n");
            }
            break;
        }
        case OPSTokenType::WRITE:
        {
            if (interpreterMagazine.size() == 0) {
                cerr << "Ошибка: недостаточно операндов для операции записи\n";
                break;
            }

            elMagazine operandForWriting = interpreterMagazine.top(); interpreterMagazine.pop();

            if (operandForWriting.contentType == ContentTypes::IntVar)
            {
                cout << "[WRITE] Вывод значения целочисленной переменной '" << operandForWriting.linkVariable << "': ";
                cout << VariableIntTable[operandForWriting.linkVariable] << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::RealVar)
            {
                cout << "[WRITE] Вывод значения вещественной переменной '" << operandForWriting.linkVariable << "': ";
                cout << VariableRealTable[operandForWriting.linkVariable] << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::IntArrayEl)
            {
                cout << "[WRITE] Вывод значения целочисленного элемента массива '" << operandForWriting.linkArrayPassport << "': ";
                cout << IntArrayTable[operandForWriting.linkArrayPassport][operandForWriting.linkArrayElement] << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::RealArrayEl)
            {
                cout << "[WRITE] Вывод значения вещественного элемента массива '" << operandForWriting.linkArrayPassport << "': ";
                cout << RealArrayTable[operandForWriting.linkArrayPassport][operandForWriting.linkArrayElement] << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::IntConst)
            {
                cout << "[WRITE] Вывод значения целочисленной константы" << ": ";
                cout << operandForWriting.intConst << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::RealConst)
            {
                cout << "[WRITE] Вывод значения вещественной константы" << ": ";
                cout << operandForWriting.realConst << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::IntResult)
            {
                cout << "[WRITE] Вывод значения целочисленного результата вычислений" << ": ";
                cout << operandForWriting.intResult << endl;
            }
            else if (operandForWriting.contentType == ContentTypes::RealResult)
            {
                cout << "[WRITE] Вывод значения вещественного результата вычислений" << ": ";
                cout << operandForWriting.realResult << endl;
            }
            else
            {
                throw runtime_error("Операнд не предназначен для чтения!\n");
            }
            break;
        }

        default:
            break;
        }

        // cout << "Номер текущего просматриваемого ОПС элемента: " << i << " , текущее содержимое магазина интерпретатора: " << endl;
        // printMagazineStack(interpreterMagazine);

    }
}

// ******************************* OPS INTERPRETER *******************************



// --- ЧТЕНИЕ КОДА (НА РАЗРАБАТЫВАЕМОМ ЯЗЫКЕ) ИЗ ФАЙЛА И ШАГИ К ЕГО КОМПИЛЯЦИИ ---
int main() {
    setlocale(0, "rus");
    // cout << "--------------------------" << "TEST LexANAL" << "--------------------------" << endl;
    lexicalAnalyzer("test.txt");

    fixUnaryMinus(tokens);

    // printLexAnalResult(tokens);

    // cout << "--------------------------" << "TEST SemanticANAL and GenOPS" << "--------------------------" << endl;
    semanticAnalyzerAndGenOPS(tokens);

    //std::cout << "LabelStack (top to bottom): ";
    //while (!LabelStack.empty()) {
    //    std::cout << LabelStack.top() << " ";
    //    LabelStack.pop();
    //}
    //std::cout << std::endl;

    /* --- Временно для теста интерпретатора ОПС --- */
    vector<OPSToken> testOPS =  // из презентации пример ОПС (арифметика) : "x a b – c d e / + * :="
    {
        {OPSTokenType::IDENTIFIER, "x"},
        {OPSTokenType::IDENTIFIER, "a"},
        {OPSTokenType::IDENTIFIER, "b"},
        {OPSTokenType::OPERATOR_MINUS, "-"},
        {OPSTokenType::IDENTIFIER, "c"},
        {OPSTokenType::IDENTIFIER, "d"},
        {OPSTokenType::IDENTIFIER, "e"},
        {OPSTokenType::OPERATOR_DIVIDE, "/"},
        {OPSTokenType::OPERATOR_PLUS, "+"},
        {OPSTokenType::OPERATOR_MULTIPLY, "*"},
        {OPSTokenType::OPERATOR_ASSIGN, "="}
    };

    // из презентации пример (массивы) : "M [ j + a ] = L [ i - d ] ;" ---> вставить в test.txt

    /* --- Временно для теста интерпретатора ОПС --- */

    cout << "--------------------------" << "TEST OPS Interpreter" << "--------------------------" << endl;
    opsInterpreter();

    // Вывод переменных
    //cout << "=== Integer Variables ===" << endl;
    //for (auto& f : VariableIntTable)
    //    cout << f.first << " : " << f.second << endl;

    //cout << "\n=== Real Variables ===" << endl;
    //for (auto& f : VariableRealTable)
    //    cout << f.first << " : " << f.second << endl;

    // Вывод паспортов массивов и содержимого
    //cout << "\n=== Array Passports & Contents ===" << endl;
    //for (auto& passportEntry : ArrayPassports) {
    //    const string& arrayName = passportEntry.first;
    //    const ArrayPassport& passport = passportEntry.second;

    //    cout << "Array: " << arrayName << ", Size: " << passport.length;

    //    // Целочисленный массив
    //    if (IntArrayTable.count(arrayName)) {
    //        cout << " (int)" << endl;
    //        auto& arr = IntArrayTable[arrayName];
    //        for (int i = 0; i < passport.length; ++i) {
    //            cout << "  [" << i << "] = " << arr[i] << endl;
    //        }
    //    }
    //    // Вещественный массив
    //    else if (RealArrayTable.count(arrayName)) {
    //        cout << " (real)" << endl;
    //        auto& arr = RealArrayTable[arrayName];
    //        for (int i = 0; i < passport.length; ++i) {
    //            cout << "  [" << i << "] = " << arr[i] << endl;
    //        }
    //    }
    //    else {
    //        cout << " (unknown type or not allocated!)" << endl;
    //    }
    //}

    return 0;
}