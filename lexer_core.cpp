// Lexer_Core.cpp

#include "lexer.hpp"
#include <cctype>

// Constructor & Destructor
Lexer::Lexer(const string& filename) {
    // Initialization of constants is delegated to the helper module
    initializeKeywordsAndOperators(); 
    sourceFile.open(filename);
    if (!sourceFile.is_open()) {
        throw runtime_error("Cannot open input file: " + filename);
    }
}

Lexer::~Lexer() {
    if (sourceFile.is_open()) {
        sourceFile.close();
    }
}

// --- Position / I/O Helpers ---

void Lexer::updatePosition(int ch) {
    if (ch == EOF) return;
    if (ch == '\n') {
        // Save current column for `retract` to restore the start of the retracted line
        if (retractStackTop < RETRACT_STACK_CAP) {
            retractColumnStack[retractStackTop++] = currentColumn;
        }
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
}

int Lexer::advance() {
    if (!isOpen()) return EOF;
    int ch = sourceFile.get();
    updatePosition(ch);
    return ch;
}

int Lexer::peek() {
    if (!isOpen()) return EOF;
    return sourceFile.peek();
}

void Lexer::retract(int ch) {
    if (!isOpen() || ch == EOF) return;

    if (!sourceFile.unget()) return;

    // Fix columns and lines
    if (ch == '\n') {
        if (retractStackTop > 0) {
            --retractStackTop;
            // Restore previous column
            currentColumn = retractColumnStack[retractStackTop];
        } else {
            currentColumn = 1; 
        }
        if (currentLine > 1) currentLine--;
    } else {
        if (currentColumn > 1) currentColumn--;
        else currentColumn = 1;
    }
}

void Lexer::getCurrentPosition(int& line, int& column) {
    line = currentLine;
    column = currentColumn;
}

// --- Main Token Loop ---

Token Lexer::getNextToken() {
    Token token;

    if (!isOpen()) {
        token.type = TokenType::END_OF_FILE;
        return token;
    }

    int ch;
    for (;;) {
        // Skip whitespace
        do {
            ch = advance();
        } while (ch != EOF && isspace(static_cast<unsigned char>(ch)));

        if (ch == EOF) {
            token.type = TokenType::END_OF_FILE;
            return token;
        }

        // Calculate token start position. Advance() moved position *past* the character, so we correct by 1.
        int startLine = currentLine;
        int startColumn = currentColumn - 1;

        // If ch was a newline, the position logic is more complex, but we rely on `updatePosition` and `retractStack` 
        // to handle column restoration on newline retraction (which is implied in the C style).
        // Since we are *not* retracting after consuming whitespace, a simple correction is usually enough.
        if (startColumn < 1) {
            // Must have been a newline that was consumed, or a file start. Use current line, col 1.
            startColumn = 1; 
        }


        // Dispatch based on the first character
        if (ch == '"' ) {
            return parseStringLiteral(ch, startLine, startColumn);
        } else if (isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
            return parseIdentifierOrKeyword(ch, startLine, startColumn);
        } else if (isdigit(static_cast<unsigned char>(ch)) || ch == '.') {
            if (ch == '.') {
                int next = peek();
                if (next == EOF || !isdigit(static_cast<unsigned char>(next))) {
                    // Treat '.' as operator (e.g., struct member access)
                    return parseOperatorOrSpecial(ch, startLine, startColumn);
                } else {
                    return parseNumber(ch, startLine, startColumn);
                }
            } else {
                return parseNumber(ch, startLine, startColumn);
            }
        } else {
            // Operator, special, or comment
            if (ch == '/') {
                int next = peek();
                if (next == '/') {
                    advance(); // Consume '/'
                    skipSingleLineComment();
                    continue; // Continue outer loop to find next token
                } else if (next == '*') {
                    advance(); // Consume '*'
                    skipMultiLineComment(startLine, startColumn);
                    continue; // Continue outer loop
                } else {
                    return parseOperatorOrSpecial(ch, startLine, startColumn);
                }
            } else {
                return parseOperatorOrSpecial(ch, startLine, startColumn);
            }
        }
    }
}