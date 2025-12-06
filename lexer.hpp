// Lexer.h

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <fstream>
#include <map>
#include <unordered_set>
#include <stdexcept>
#include <iostream>

using namespace std;
// --- Color definitions for output ---
#define C_RESET  "\x1b[0m"
#define C_KEY    "\x1b[35m" /* magenta */
#define C_ID     "\x1b[36m" /* cyan */
#define C_CONST  "\x1b[32m" /* green */
#define C_OP     "\x1b[31m" /* red */
#define C_SPEC   "\x1b[33m" /* yellow */

// --- Constants ---
constexpr int MAX_TOKEN_SIZE = 256;
constexpr int RETRACT_STACK_CAP = 65536;

// --- Token Types ---
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    OPERATOR,
    SPECIAL_CHARACTER,
    UNKNOWN,
    END_OF_FILE
};

// --- Token Structure ---
struct Token {
    TokenType type = TokenType::UNKNOWN;
    string lexeme;
    int line = 0;    // starting line (1-based)
    int column = 0;  // starting column (1-based)

    // Helper methods (Implementation in Lexer_Helpers.cpp)
    string getTypeName() const;
    string getColor() const;
};

// --- Lexer Class ---
class Lexer {
public:
    // Public API (Implemented in Lexer_Core.cpp)
    explicit Lexer(const string& filename);
    ~Lexer();
    Token getNextToken();

private:
    // Core State (Implemented in Lexer_Core.cpp)
    ifstream sourceFile;
    int currentLine = 1;
    int currentColumn = 1;
    int retractColumnStack[RETRACT_STACK_CAP];
    int retractStackTop = 0;

    // Constants/Helper State (Implemented in Lexer_Helpers.cpp)
    unordered_set<string> keywords;
    unordered_set<char> singleOperatorChars;
    unordered_set<char> specialChars;
    unordered_set<string> multiOperators;
    
    // Core I/O Helpers (Implemented in Lexer_Core.cpp)
    int advance();
    int peek();
    void retract(int ch);
    void getCurrentPosition(int& line, int& column);
    void updatePosition(int ch);

    // Helpers/Categorization (Implemented in Lexer_Helpers.cpp)
    void initializeKeywordsAndOperators();
    void reportLexerError(int line, int column, const string& message, const string& lexeme = "");
    void categorizeToken(Token& token);
    bool isKeyword(const string& str) const;
    bool isOperator(const string& str) const;
    bool isConstant(const string& str) const;
    bool isIdentifier(const string& str) const;

    // Parsing Functions (Implemented in Lexer_Constants.cpp and Lexer_Operators.cpp)
    Token parseIdentifierOrKeyword(int ch_input, int startLine, int startColumn); // For Identifiers and Keywords
    Token parseNumber(int ch_input, int startLine, int startColumn);               // For Numeric and Float Constants
    Token parseStringLiteral(int ch_input, int startLine, int startColumn);        // For String Literals
    Token parseOperatorOrSpecial(int ch_input, int startLine, int startColumn);    // For Operators and Specials
    void skipSingleLineComment();                                                  // For Comments
    void skipMultiLineComment(int startLine, int startColumn);                     // For Comments

    // Internal state management
    bool isOpen() const { return sourceFile.is_open(); }
};

#endif // LEXER_H