// main.cpp

#include "lexer.hpp"
#include <iostream>
#include <iomanip>
#include <cstdlib>

int main(int argc, char *argv[]) {
    string filename;

    if (argc >= 2) {
        filename = argv[1];
    } else {
        cout << "Enter source file name: ";
        if (!(cin >> filename)) {
            cerr << "Failed to read filename\n";
            return 2;
        }
    }

    try {
        Lexer lexer(filename);
        
        // Output formatting exactly as requested (based on original C code)
        cout << "\n| " << left << setw(12) << "Position" 
                  << " | " << setw(20) << "Lexeme" 
                  << " | " << setw(18) << "Type" << " |\n";
        cout << "|--------------|----------------------|--------------------|\n";

        while (true) {
            Token t = lexer.getNextToken();
            if (t.type == TokenType::END_OF_FILE) break;

            string col = t.getColor();
            
            // Format for position (l:LLL c:CCC)
            cout << "| l:" << left << setw(3) << t.line
                      << " c:" << setw(3) << t.column << "  "
                      << " | " << col << left << setw(20) << t.lexeme << C_RESET
                      << " | " << col << setw(18) << t.getTypeName() << C_RESET << " |\n";
        }
        
    } catch (const exception& e) {
        cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}