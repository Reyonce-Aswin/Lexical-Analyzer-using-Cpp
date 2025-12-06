// Lexer_Operators.cpp

#include "lexer.hpp"
#include <cctype>

// --- Comment Handlers ---

void Lexer::skipSingleLineComment() {
    int ch;
    // Advance until we hit EOF or a newline
    while ((ch = advance()) != EOF && ch != '\n') { /* skip */ }
}

void Lexer::skipMultiLineComment(int startLine, int startColumn) {
    int prev = 0;
    int ch;
    bool ended = false;
    
    // '*' is already consumed after seeing '/' in getNextToken
    while ((ch = advance()) != EOF) {
        if (prev == '*' && ch == '/') {
            ended = true;
            break;
        }
        prev = ch;
    }
    
    if (!ended) {
        int errLine, errCol;
        getCurrentPosition(errLine, errCol);
        reportLexerError(errLine, errCol, "Unclosed multi-line comment at EOF.");
    }
}

// --- Operator/Special Parsing ---

Token Lexer::parseOperatorOrSpecial(int ch_input, int startLine, int startColumn) {
    Token token;
    token.line = startLine;
    token.column = startColumn;
    string lexeme;

    int ch = ch_input;

    lexeme.push_back((char)ch);

    // Check multi-character operators using peek
    int next = peek();
    if (next != EOF) {
        string two_chars;
        two_chars.push_back((char)ch);
        two_chars.push_back((char)next);

        if (isOperator(two_chars)) {
            advance(); // Consume next
            lexeme.push_back((char)next);
        }
    }

    token.lexeme = lexeme;
    categorizeToken(token);

    if (token.type == TokenType::UNKNOWN) {
        reportLexerError(token.line, token.column, "Unrecognized token", token.lexeme);
    }

    return token;
}