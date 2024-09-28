// File created by fob

#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <ostream>
#include <string>
#include <unordered_map>

// The Lexer class is responsible for tokenizing the input source code.
// It processes the input file and converts it into a sequence of tokens
// that will be used by the parser.
class Lexer {
public:
    // Enumeration of possible tokens in the language
    // Each token represents a type of symbol or keyword.
    enum class Token {
        NUM, ID, INT, BOOLEAN, TRUE, FALSE, LBRACE, RBRACE,
        LBRACKET, RBRACKET, LPARENTHESIS, RPARENTHESIS, SEMICOLON,
        IF, ELSE, WHILE, DO, BREAK, PRINT, EQ, NEQ, AND, OR, LESS,
        LESSEQ, GREATER, GREATEREQ, ASSIGN, PLUS, MINUS, MULTIPLY,
        DIVIDE, NOT, END, ERROR
    };

    // Constructor: Initializes the lexer with an input file stream.
    // It also sets up the map for reserved keywords and reads the first character.
    Lexer(std::ifstream &input);

    // Returns the next token from the input stream.
    Token nextToken();

    // Accessors for the current token's value:
    // - Returns the value of the number token if the current token is of type NUM.
    int getNumber() const;

    // - Returns the value of the identifier token if the current token is of type ID.
    std::string getIdentifier() const;

    // Utility methods for debugging and error reporting:
    // - Returns the current line being processed.
    int getLine() const;

    // - Returns the current column being processed.
    int getColumn() const;

private:
    // Private variables for lexing state:
    std::ifstream &input;                            // Reference to the input file stream
    char currentCharacter;                           // Current character being analyzed
    std::unordered_map<std::string, Token> keywords; // Reserved keywords in the language

    // Tracking line and column numbers for error reporting
    int line = 1;        // Current line number
    int column = 0;      // Current column number

    // Variables for the values of current tokens
    int numberValue;              // Holds the numeric value of NUM tokens
    std::string identifierValue;  // Holds the string value of ID tokens

    // Utility methods for lexing:
    // - Reads the next character from the input file.
    void advance();

    // - Skips whitespace characters (spaces, tabs, newlines).
    void skipWhitespace();

    // - Parses and returns a numeric token (NUM).
    Token scanNumber();

    // - Parses and returns either an identifier token (ID) or a keyword token.
    Token scanIdentifierOrKeyword();
};

// Utility functions for debugging and pretty-printing:

// Converts the current state of the Lexer to a human-readable string.
// Useful for debugging purposes.
std::string to_string(const Lexer &lexer);

// Converts a token to a human-readable string.
// Useful for printing tokens during debugging or logging.
std::string to_string(const Lexer::Token token);

#endif // LEXER_H
