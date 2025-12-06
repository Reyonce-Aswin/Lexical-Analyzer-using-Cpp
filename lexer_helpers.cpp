// Lexer_Helpers.cpp

#include "lexer.hpp"
#include <cctype>
#include <iomanip>

// --- Token Helper Implementations (Moved from previous main.cpp) ---

string Token::getTypeName() const {
    switch (type) {
        case TokenType::KEYWORD: return "keyword";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::CONSTANT: return "constant";
        case TokenType::OPERATOR: return "operator";
        case TokenType::SPECIAL_CHARACTER: return "special character";
        case TokenType::UNKNOWN: return "unknown";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "invalid";
    }
}

string Token::getColor() const {
    switch (type) {
        case TokenType::KEYWORD: return C_KEY;
        case TokenType::IDENTIFIER: return C_ID;
        case TokenType::CONSTANT: return C_CONST;
        case TokenType::OPERATOR: return C_OP;
        case TokenType::SPECIAL_CHARACTER: return C_SPEC;
        default: return C_RESET;
    }
}

// --- Lexer Initialization (Replaces C static arrays) ---

void Lexer::initializeKeywordsAndOperators() {
    const string KW_LIST[] = {
        "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
        "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct",
        "ifndef", "define", "include", "endif"
    };
    for (const auto& kw : KW_LIST) {
        keywords.insert(kw);
    }

    const string OP_CHARS = "+-*/%=!<>|&^~.:";
    for (char ch : OP_CHARS) {
        singleOperatorChars.insert(ch);
    }
    
    const string SP_CHARS = ",;{}()[]#";
    for (char ch : SP_CHARS) {
        specialChars.insert(ch);
    }

    const string MULTI_OP_LIST[] = {
        "==", "!=", "<=", ">=", "++", "--", "&&", "||",
        "+=", "-=", "*=", "/=", "%=", "<<", ">>", "->", "::"
    };
    for (const auto& op : MULTI_OP_LIST) {
        multiOperators.insert(op);
    }
}

// --- Categorization Helpers ---

bool Lexer::isKeyword(const string& str) const {
    // O(1) average time complexity search with unordered_set
    return keywords.count(str);
}

bool Lexer::isOperator(const string& str) const {
    if (str.length() == 1) {
        return singleOperatorChars.count(str[0]);
    } else if (str.length() == 2) {
        return multiOperators.count(str);
    }
    return false;
}

bool Lexer::isConstant(const string& str) const {
    if (str.empty()) return false;
    
    // Quick check for string literal
    if (str.front() == '"' && str.back() == '"') return true;
    
    int dotCount = 0;
    bool digits = false;
    
    size_t i = 0;
    if (str[i] == '.') {
        dotCount++;
        i++;
        if (i == str.length() || !isdigit(static_cast<unsigned char>(str[i]))) {
            return false; // must be followed by digit if starting with '.'
        }
    }

    for (; i < str.length(); ++i) {
        if (isdigit(static_cast<unsigned char>(str[i]))) {
            digits = true;
            continue;
        }
        if (str[i] == '.') {
            dotCount++;
            if (dotCount > 1) return false;
            continue;
        }
        return false;
    }
    return digits;
}

bool Lexer::isIdentifier(const string& str) const {
    if (str.empty()) return false;
    if (!(isalpha(static_cast<unsigned char>(str[0])) || str[0] == '_')) return false;
    for (size_t i = 1; i < str.length(); ++i) {
        if (!isalnum(static_cast<unsigned char>(str[i])) && str[i] != '_') return false;
    }
    return true;
}

void Lexer::categorizeToken(Token& token) {
    if (token.lexeme.empty()) {
        token.type = TokenType::UNKNOWN;
        return;
    }
    
    if (token.lexeme.front() == '"') {
        token.type = TokenType::CONSTANT; // String literals are constants
        return;
    }

    if (isKeyword(token.lexeme)) {
        token.type = TokenType::KEYWORD;
        return;
    }
    if (isOperator(token.lexeme)) {
        token.type = TokenType::OPERATOR;
        return;
    }
    if (token.lexeme.length() == 1 && specialChars.count(token.lexeme[0])) {
        token.type = TokenType::SPECIAL_CHARACTER;
        return;
    }
    if (isConstant(token.lexeme)) {
        token.type = TokenType::CONSTANT;
        return;
    }
    if (isIdentifier(token.lexeme)) {
        token.type = TokenType::IDENTIFIER;
        return;
    }
    token.type = TokenType::UNKNOWN;
}

// --- Error Reporting ---

void Lexer::reportLexerError(int line, int column, const string& message, const string& lexeme) {
    cerr << "\x1b[1m\x1b[31mError\x1b[0m: ("
              << line << ", " << column << ") "
              << message 
              << (lexeme.empty() ? "" : ": \"" + lexeme + "\"")
              << "\x1b[0m\n";
}