// File created by fob

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

// The Parser is responsible for transforming tokens generated by the Lexer into
// an Abstract Syntax Tree (AST) according to the defined grammar rules of the language.
class Parser {
public:
    // Constructor: Initializes the parser with a reference to the Lexer
    // and prepares to start parsing from the first token.
    Parser(Lexer &lexer);

    // Starts the parsing process and returns the root node of the AST.
    // This is the main entry point for the parser.
    Node* parse();

private:
    Lexer &lexer;               // Reference to the Lexer that provides tokens.
    Lexer::Token currentToken;  // Holds the current token being processed.

    // Helper Functions

    // Advances the parser to the next token by asking the Lexer for the next token.
    // Ensures `currentToken` is always the most up-to-date token.
    void advance();

    // Matches the current token with the expected token.
    // If they match, it advances to the next token, otherwise, it throws an error.
    void match(Lexer::Token expected);

    // Grammar Rules: These functions represent the parsing rules for different grammar constructs.
    // Each function corresponds to a non-terminal symbol in the grammar

    // <program> -> <block>
    // The main entry point for the language's structure.
    Node* parseProgram();

    // <block> -> { <decls> <stmts> }
    // Parses a block of code, handling declarations and statements within `{}` braces.
    Node* parseBlock();

    // <decls> -> <decl> <decls> | null
    // Parses a series of declarations. Can be empty (null).
    Node* parseDecls();

    // <decl> -> <type> id ;
    // Parses a single declaration with a type and an identifier.
    Node* parseDecl();

    // <type> -> <type> [ num ] | <basic>
    // Parses a type declaration, including array types and basic types like `int` and `boolean`.
    Node* parseType();

    // <basic> -> int | boolean
    // Parses basic types such as `int` and `boolean`.
    Node* parseBasic();

    // <stmts> -> <stmt> <stmts> | null
    // Parses a sequence of statements. Can be empty (null).
    Node* parseStmts();

    // <stmt> -> <loc> = <bool> ;
    //         | if ( <bool> ) <stmt>
    //         | if ( <bool> ) <stmt> else <stmt>
    //         | while ( <bool> ) <stmt>
    //         | do <stmt> while ( <bool> ) ;
    //         | break ;
    //         | print ( <bool> ) ;
    //         | <block>
    // Parses individual statements such as assignments, conditionals, loops, and print calls.
    Node* parseStmt();

    // <loc> -> <loc> [ <bool> ] | id
    // Parses a location in memory, either a variable or an array access.
    Node* parseLoc();

    // <bool> -> <bool> || <join> | <join>
    // Parses boolean expressions using logical OR.
    Node* parseBool();

    // <join> -> <join> && <equality> | <equality>
    // Parses boolean expressions using logical AND.
    Node* parseJoin();

    // <equality> -> <equality> == <rel> | <equality> != <rel> | <rel>
    // Parses equality comparisons (==, !=).
    Node* parseEquality();

    // <rel> -> <expr> < <expr>
    //        | <expr> <= <expr>
    //        | <expr> >= <expr>
    //        | <expr> > <expr>
    //        | <expr>
    // Parses relational expressions (>, <, <=, >=).
    Node* parseRel();

    // <expr> -> <expr> + <term> | <expr> - <term> | <term>
    // Parses expressions involving addition and subtraction.
    Node* parseExpr();

    // <term> -> <term> * <unary> | <term> / <unary> | <unary>
    // Parses terms involving multiplication and division.
    Node* parseTerm();

    // <unary> -> ! <unary> | - <unary> | <factor>
    // Parses unary operators such as negation and logical NOT.
    Node* parseUnary();

    // <factor> -> ( <bool> ) | <loc> | num | true | false
    // Parses individual factors such as parenthesized expressions, variables, numbers, or boolean literals.
    Node* parseFactor();
};

#endif // PARSER_H
