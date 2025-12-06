// Lexer_Constants.cpp

#include "lexer.hpp"
#include <cctype>

// --- Parsing Functions for Identifiers and Constants ---

Token Lexer::parseIdentifierOrKeyword(int ch_input, int startLine, int startColumn) {
    Token token;
    token.line = startLine;
    token.column = startColumn;
    string lexeme;

    int ch = ch_input;
    lexeme.push_back((char)ch);

    // Consume all subsequent alphanumeric characters and underscores
    while ((ch = peek()) != EOF && (isalnum(static_cast<unsigned char>(ch)) || ch == '_')) {
        advance();
        if (lexeme.length() < MAX_TOKEN_SIZE - 1) {
            lexeme.push_back((char)ch);
        } else {
            // Report overflow and consume remainder silently
            reportLexerError(startLine, startColumn, "Identifier too long (truncated)");
            while ((ch = peek()) != EOF && (isalnum(static_cast<unsigned char>(ch)) || ch == '_')) {
                advance();
            }
            break;
        }
    }

    token.lexeme = lexeme;
    categorizeToken(token); // Determines if it's KEYWORD or IDENTIFIER
    return token;
}

Token Lexer::parseNumber(int ch_input, int startLine, int startColumn) {
    Token token;
    token.line = startLine;
    token.column = startColumn;
    string lexeme;
    int ch = ch_input;
    int dotCount = 0;

    lexeme.push_back((char)ch);
    if (ch == '.') dotCount++;

    while (true) {
        int n = peek();
        if (n == EOF || !(isdigit(static_cast<unsigned char>(n)) || n == '.')) {
            break;
        }

        if (n == '.') {
            dotCount++;
            if (dotCount > 1) break; 
        }

        advance(); 
        ch = n;
        
        if (lexeme.length() < MAX_TOKEN_SIZE - 1) {
            lexeme.push_back((char)ch);
        } else {
            reportLexerError(startLine, startColumn, "Numeric token too long (truncated)");
            // Consume rest of digits/dots
            while (peek() != EOF && (isdigit(static_cast<unsigned char>(peek())) || peek() == '.')) advance();
            break;
        }
    }

    token.lexeme = lexeme;
    categorizeToken(token);
    return token;
}

Token Lexer::parseStringLiteral(int ch_input, int startLine, int startColumn) {
    Token token;
    token.line = startLine;
    token.column = startColumn;
    string lexeme;
    
    // Store opening quote
    lexeme.push_back((char)ch_input);

    bool closed = false;
    int ch;
    while (true) {
        ch = advance();

        if (ch == EOF) {
            token.lexeme = lexeme;
            reportLexerError(startLine, startColumn, "Unclosed string literal at EOF", token.lexeme);
            return token;
        }
        if (ch == '\n') {
            lexeme.push_back((char)ch);
            token.lexeme = lexeme;
            reportLexerError(startLine, startColumn, "String literal not terminated before newline", token.lexeme);
            return token;
        }
        
        // Handle escape: store backslash and next char
        if (ch == '\\') {
            lexeme.push_back((char)ch);
            int esc = advance();
            if (esc == EOF) {
                token.lexeme = lexeme;
                reportLexerError(startLine, startColumn, "Unclosed string literal after escape at EOF", token.lexeme);
                return token;
            }
            if (lexeme.length() < MAX_TOKEN_SIZE - 1) lexeme.push_back((char)esc);
            continue;
        }
        
        if (ch == '"') {
            if (lexeme.length() < MAX_TOKEN_SIZE - 1) lexeme.push_back((char)ch);
            closed = true;
            break;
        }
        
        if (lexeme.length() < MAX_TOKEN_SIZE - 1) {
            lexeme.push_back((char)ch);
        } else {
            reportLexerError(startLine, startColumn, "String literal too long (truncated)");
            // Consume rest until closing quote or EOF
            while ((ch = advance()) != EOF && ch != '"') {}
            if (ch == '"') {
                if (lexeme.length() < MAX_TOKEN_SIZE - 1) lexeme.push_back((char)ch);
                closed = true;
            } else {
                reportLexerError(startLine, startColumn, "Unclosed string literal at EOF", lexeme);
                token.lexeme = lexeme;
                return token;
            }
            break;
        }
    }

    token.lexeme = lexeme;
    if (closed) categorizeToken(token);
    else token.type = TokenType::UNKNOWN;
    return token;
}