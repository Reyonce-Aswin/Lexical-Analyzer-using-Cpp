# Lexical Analyzer using C++

## Overview

This project implements a modular **Lexer (Tokenizer)** in modern C++.\
The lexer reads a source file, processes characters one by one, and
produces a sequence of tokens such as identifiers, keywords, literals,
operators, and punctuation.

The implementation is cleanly split across multiple source files for
maintainability and extensibility.

## Project Structure

    .
    ├── lexer.hpp             # Public API: Lexer class, Token struct, TokenType enum
    ├── lexer_core.cpp        # Core scanning logic & main tokenization loop
    ├── lexer_operators.cpp   # Operator token parsing (+=, ==, <=, &&, ||, etc.)
    ├── lexer_constants.cpp   # Identifier / keyword / numeric constant handling
    ├── lexer_helpers.cpp     # Utility functions: char checks, string building, etc.
    ├── main.cpp              # Example entry point demonstrating how to use the lexer

## Features

- Tokenizes:
    - Identifiers and keywords\
    - Numeric literals (int/float)\
    - String literals\
    - Operators (`+`, `-`, `==`, `!=`, `<=`, `&&`, etc.)\
    - Delimiters (`(`, `)`, `{`, `}`, `;`, `,` etc.)
-   Supports:
    - Whitespace skipping\
    - `//` single-line comments\
    - `/* ... */` multi-line comments\
- Tracks accurate **line** and **column** numbers\
- Clean separation of token logic across multiple files\
- Error handling for invalid characters, malformed literals, and
    unterminated strings/comments

## How the Lexer Works

1. The **core** logic in `lexer_core.cpp` drives scanning using a
    `getNextToken()` loop.\
2. Token-specific logic is delegated to helper modules:
    - `lexer_operators.cpp` handles single/multi-character operators\
    - `lexer_constants.cpp` handles identifiers, keywords, numbers,
        strings\
    - `lexer_helpers.cpp` contains reusable character and lexeme
        utilities\
3. The lexer constructs and returns a `Token` object containing:
    - TokenType\
    - Lexeme\
    - Line & Column\
4. The stream ends with `TokenType::END_OF_FILE`.

## Example Usage (main.cpp)

``` cpp
#include "lexer.hpp"
#include <iostream>

int main() {
    Lexer lexer("input.txt");

    while (true) {
        Token tok = lexer.getNextToken();
        std::cout << tok.lexeme << "  (" 
                  << tokenTypeToString(tok.type) << ")  @ "
                  << tok.line << ":" << tok.column << "
";

        if (tok.type == TokenType::END_OF_FILE)
            break;
    }

    return 0;
}
```

## Example Output

    var (KEYWORD) @ 1:1
    x (IDENTIFIER) @ 1:5
    = (ASSIGN) @ 1:7
    42 (NUMBER) @ 1:9
    ; (SEMICOLON) @ 1:11

## Supported Token Types

- Identifiers\
- Keywords\
- Integer/Float literals\
- String literals\
- Operators (arithmetic, comparison, logical, assignment)\
- Delimiters\
- Comments (`//`, `/* ... */`)\
- END_OF_FILE

## Error Handling

The lexer provides descriptive error messages:

    Error: Unterminated string literal at line 4, column 15
    Error: Unexpected character '@' at line 2, column 9
    Error: Unterminated block comment at line 10

## Building the Project

### Compile using g++

    g++ *.cpp -o lexer

### Run

    ./lexer input.txt

## Extending the Lexer

### Adding a new keyword

Modify the keyword map inside **lexer_constants.cpp**.

### Adding a new operator

Extend the operator recognition logic in **lexer_operators.cpp**.

### New token category

1. Add it to `enum TokenType` in **lexer.hpp**\
2. Add handling logic in the appropriate `.cpp`\
3. Update `tokenTypeToString()` in **lexer.hpp**

---

## Author

**Reyonce Aswin T**  
*Student ID: 25021_181*  
Embedded Systems Student, Emertxe Information Technologies, Bangalore

---
