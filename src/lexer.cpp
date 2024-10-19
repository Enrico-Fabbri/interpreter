// File created by fob

#include "../include/lexer.h"

#include <cctype>

// Constructor: Initializes the lexer and reads the first character from input.
Lexer::Lexer(std::ifstream &input) : input(input) {
    // Initialize reserved keywords in the language.
    keywords = {
            {"int", Token::INT},
            {"boolean", Token::BOOLEAN},
            {"true", Token::TRUE},
            {"false", Token::FALSE},
            {"if", Token::IF},
            {"else", Token::ELSE},
            {"while", Token::WHILE},
            {"do", Token::DO},
            {"break", Token::BREAK},
            {"print", Token::PRINT},
    };

    // Start lexing by reading the first character.
    advance();
}

// Advances to the next character in the input file stream.
// Handles line and column tracking for error reporting.
void Lexer::advance() {
    if (input.get(currentCharacter)) {
        // Update position: reset column number on new lines.
        if (currentCharacter == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
    } else {
        // If input stream ends, set current character to null character.
        currentCharacter = '\0';
    }
}

// Skips whitespace (spaces, tabs, newlines) to find the next meaningful character.
void Lexer::skipWhitespace() {
    while (isspace(currentCharacter)) {
        advance();
    }
}

// Recognizes and processes a numeric token. Supports integer numbers.
// Advances the lexer through the input stream until a non-digit is found.
Lexer::Token Lexer::scanNumber() {
    numberValue = 0;

    // Loop over the digits and build the integer value.
    while (isdigit(currentCharacter)) {
        numberValue = numberValue * 10 + (currentCharacter - '0');
        advance();
    }

    return Token::NUM;
}

// Recognizes and processes either an identifier or a keyword token.
// If the lexeme matches a keyword, returns the appropriate token.
// Otherwise, returns Token::ID for identifiers.
Lexer::Token Lexer::scanIdentifierOrKeyword() {
    identifierValue.clear();

    // Build identifier from alphanumeric characters.
    while (isalnum(currentCharacter)) {
        identifierValue += currentCharacter;
        advance();
    }

    // Check if the lexeme is a keyword, otherwise it's an identifier.
    auto keyword = keywords.find(identifierValue);
    if (keyword != keywords.end()) {
        return keyword->second;
    }
    return Token::ID;
}

// Main function to fetch the next token from the input stream.
// Skips over whitespaces and matches single or multi-character tokens.
Lexer::Token Lexer::nextToken() {
    skipWhitespace();

    // If the current character is a digit, it's part of a number.
    if (isdigit(currentCharacter)) {
        return scanNumber();
    }
    // If it's a letter, it could be an identifier or a keyword.
    else if (isalpha(currentCharacter)) {
        return scanIdentifierOrKeyword();
    }
    // Handle single and multi-character operators and delimiters.
    else {
        switch (currentCharacter) {
            case '+': advance(); return Token::PLUS;
            case '-': advance(); return Token::MINUS;
            case '*': advance(); return Token::MULTIPLY;
            case '/': advance(); return Token::DIVIDE;
            case '=':
                advance();
                if (currentCharacter == '=') {
                    advance();
                    return Token::EQ;
                }
                return Token::ASSIGN;
            case '!':
                advance();
                if (currentCharacter == '=') {
                    advance();
                    return Token::NEQ;
                }
                return Token::NOT;
            case '<':
                advance();
                if (currentCharacter == '=') {
                    advance();
                    return Token::LESSEQ;
                }
                return Token::LESS;
            case '>':
                advance();
                if (currentCharacter == '=') {
                    advance();
                    return Token::GREATEREQ;
                }
                return Token::GREATER;
            case '&':
                advance();
                if (currentCharacter == '&') {
                    advance();
                    return Token::AND;
                }
                return Token::ERROR;
            case '|':
                advance();
                if (currentCharacter == '|') {
                    advance();
                    return Token::OR;
                }
                return Token::ERROR;
            case ';': advance(); return Token::SEMICOLON;
            case '{': advance(); return Token::LBRACE;
            case '}': advance(); return Token::RBRACE;
            case '[': advance(); return Token::LBRACKET;
            case ']': advance(); return Token::RBRACKET;
            case '(': advance(); return Token::LPARENTHESIS;
            case ')': advance(); return Token::RPARENTHESIS;
            case '\0': return Token::END;
            default: return Token::ERROR;
        }
    }
}

// Returns the numeric value of the current number token.
int Lexer::getNumber() const {
    return numberValue;
}

// Returns the string value of the current identifier token.
std::string Lexer::getIdentifier() const {
    return identifierValue;
}

// Returns the current line number.
int Lexer::getLine() const {
    return line;
}

// Returns the current column number.
int Lexer::getColumn() const {
    return column;
}

// Converts the current state of the lexer (line and column) to a string.
std::string to_string(const Lexer &lexer) {
    return "line: " + std::to_string(lexer.getLine()) + " column: " + std::to_string(lexer.getColumn());
}

// Converts a Token enum to its string representation.
std::string to_string(const Lexer::Token token) {
    switch (token) {
        case Lexer::Token::NUM:          return "NUM";
        case Lexer::Token::ID:           return "ID";
        case Lexer::Token::INT:          return "INT";
        case Lexer::Token::BOOLEAN:      return "BOOLEAN";
        case Lexer::Token::TRUE:         return "TRUE";
        case Lexer::Token::FALSE:        return "FALSE";
        case Lexer::Token::LBRACE:       return "LBRACE";
        case Lexer::Token::RBRACE:       return "RBRACE";
        case Lexer::Token::LBRACKET:     return "LBRACKET";
        case Lexer::Token::RBRACKET:     return "RBRACKET";
        case Lexer::Token::LPARENTHESIS: return "LPARENTHESIS";
        case Lexer::Token::RPARENTHESIS: return "RPARENTHESIS";
        case Lexer::Token::SEMICOLON:    return "SEMICOLON";
        case Lexer::Token::IF:           return "IF";
        case Lexer::Token::ELSE:         return "ELSE";
        case Lexer::Token::WHILE:        return "WHILE";
        case Lexer::Token::DO:           return "DO";
        case Lexer::Token::BREAK:        return "BREAK";
        case Lexer::Token::PRINT:        return "PRINT";
        case Lexer::Token::EQ:           return "EQ";
        case Lexer::Token::NEQ:          return "NEQ";
        case Lexer::Token::AND:          return "AND";
        case Lexer::Token::OR:           return "OR";
        case Lexer::Token::LESS:         return "LESS";
        case Lexer::Token::LESSEQ:       return "LESSEQ";
        case Lexer::Token::GREATER:      return "GREATER";
        case Lexer::Token::GREATEREQ:    return "GREATEREQ";
        case Lexer::Token::ASSIGN:       return "ASSIGN";
        case Lexer::Token::PLUS:         return "PLUS";
        case Lexer::Token::MINUS:        return "MINUS";
        case Lexer::Token::MULTIPLY:     return "MULTIPLY";
        case Lexer::Token::DIVIDE:       return "DIVIDE";
        case Lexer::Token::NOT:          return "NOT";
        case Lexer::Token::END:          return "END";
        case Lexer::Token::ERROR:        return "ERROR";
    }
    return "UNKNOWN";
}
