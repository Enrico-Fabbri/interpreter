// File created by fob

#include "../include/parser.h"

// Constructor: Initializes the parser with a lexer and advances to the first token.
Parser::Parser(Lexer &lexer) : lexer(lexer) {
    advance();  // Load the first token from the lexer.
}

// Utility function: Advances the lexer to the next token.
void Parser::advance() {
    currentToken = lexer.nextToken();  // Fetch the next token from the lexer.
}

// Utility function: Matches the current token with an expected token.
// If the match fails, throws a runtime error with a detailed message.
void Parser::match(Lexer::Token expected) {
    if (currentToken == expected) {
        advance();
    } else {
        std::string errMsg = "Error: Unexpected token " + to_string(currentToken) + " at " + to_string(lexer) + ". Expected " + to_string(expected);
        throw std::runtime_error(errMsg);
    }
}

// Main entry point for parsing: Starts the parsing process and returns the AST.
Node* Parser::parse() {
    return parseProgram();
}

// <program> -> <block>
Node *Parser::parseProgram() {
    Node *block = parseBlock();

    return new ProgramNode(lexer, block);
}

// <block> -> { <decls> <stmts> }
Node *Parser::parseBlock() {
    match(Lexer::Token::LBRACE);
    Node* decls = parseDecls();
    Node* stmts = parseStmts();
    match(Lexer::Token::RBRACE);

    return new BlockNode(lexer, decls, stmts);
}

// <decls> -> <decl> <decls> | null
Node *Parser::parseDecls() {
    if (currentToken == Lexer::Token::INT || currentToken == Lexer::Token::BOOLEAN) {
        Node *decl = parseDecl();
        Node *next = parseDecls();

        return new DeclsNode(lexer, decl, next);
    } else {
        return nullptr;
    }
}

// <decl> -> <type> id ;
Node *Parser::parseDecl() {
    Node *type = parseType();

    std::string id = lexer.getIdentifier();
    match(Lexer::Token::ID);
    match(Lexer::Token::SEMICOLON);

    return new DeclNode(lexer, type, id);
}

// <type> -> <type> [ num ] | <basic>
Node *Parser::parseType() {
    Node *basicType = parseBasic();

    // If the type is an array, match the array brackets and size.
    if (currentToken == Lexer::Token::LBRACKET) {
        match(Lexer::Token::LBRACKET);
        int arraySize = lexer.getNumber();
        match(Lexer::Token::NUM);
        match(Lexer::Token::RBRACKET);

        return new ArrayTypeNode(lexer, basicType, arraySize);
    }

    return basicType;
}

// <basic> -> int | boolean
Node *Parser::parseBasic() {
    if (currentToken == Lexer::Token::INT) {
        match(Lexer::Token::INT);

        return new BasicTypeNode(lexer, "integer");
    } else if (currentToken == Lexer::Token::BOOLEAN) {
        match(Lexer::Token::BOOLEAN);

        return new BasicTypeNode(lexer, "boolean");
    }

    std::string errMsg = "Error: Expected 'int' or 'bool' at " + to_string(lexer) + ". Found " + to_string(currentToken);
    throw std::runtime_error(errMsg);
}

// <stmts> -> <stmt> <stmts> | null
Node *Parser::parseStmts() {
    Node *stmt;
    Node *next;

    switch (currentToken) {
        case Lexer::Token::ID:
        case Lexer::Token::IF:
        case Lexer::Token::WHILE:
        case Lexer::Token::DO:
        case Lexer::Token::BREAK:
        case Lexer::Token::PRINT:
        case Lexer::Token::LBRACE:
            stmt = parseStmt();
            next = parseStmts();

            return new StmtsNode(lexer, stmt, next);
        default:
            return nullptr;
    }
}

// <stmt> -> <loc> = <bool> ; | if ( <bool> ) <stmt> | if ( <bool> ) <stmt> else
// <stmt> | while ( <bool> ) <stmt> | do <stmt> while ( <bool> ) ; | break ; |
// print ( <bool> ) ; | <block>
Node *Parser::parseStmt() {
    Node *loc, *expr, *condition, *ifStmt, *elseStmt, *body;

    switch (currentToken) {
        case Lexer::Token::ID:
            loc = parseLoc();
            match(Lexer::Token::ASSIGN);
            expr = parseBool();
            match(Lexer::Token::SEMICOLON);

            return new AssignNode(lexer, loc, expr);
        case Lexer::Token::IF:
            match(Lexer::Token::IF);
            match(Lexer::Token::LPARENTHESIS);
            condition = parseBool();
            match(Lexer::Token::RPARENTHESIS);
            ifStmt = parseStmt();

            if (currentToken == Lexer::Token::ELSE) {
                match(Lexer::Token::ELSE);
                elseStmt = parseStmt();

                return new IfElseNode(lexer, condition, ifStmt, elseStmt);
            }

            return new IfNode(lexer, condition, ifStmt);
        case Lexer::Token::WHILE:
            match(Lexer::Token::WHILE);
            match(Lexer::Token::LPARENTHESIS);
            condition = parseBool();
            match(Lexer::Token::RPARENTHESIS);
            body = parseStmt();

            return new WhileNode(lexer, condition, body);
        case Lexer::Token::DO:
            match(Lexer::Token::DO);
            body = parseStmt();
            match(Lexer::Token::WHILE);
            match(Lexer::Token::LPARENTHESIS);
            condition = parseBool();
            match(Lexer::Token::RPARENTHESIS);
            match(Lexer::Token::SEMICOLON);

            return new DoWhileNode(lexer, body, condition);
        case Lexer::Token::BREAK:
            match(Lexer::Token::BREAK);
            match(Lexer::Token::SEMICOLON);

            return new BreakNode(lexer);
        case Lexer::Token::PRINT:
            match(Lexer::Token::PRINT);
            match(Lexer::Token::LPARENTHESIS);
            expr = parseBool();
            match(Lexer::Token::RPARENTHESIS);
            match(Lexer::Token::SEMICOLON);

            return new PrintNode(lexer, expr);
        case Lexer::Token::LBRACE:
            return parseBlock();
        default:
            std::string errMsg = "Error: Invalid statement at " + to_string(lexer);
            throw std::runtime_error(errMsg);
    }
}

// <loc> -> <loc> [ <bool> ] | id
Node *Parser::parseLoc() {
    std::string id = lexer.getIdentifier();
    match(Lexer::Token::ID);

    if (currentToken == Lexer::Token::LBRACKET) {
        match(Lexer::Token::LBRACKET);
        Node *indexExpr = parseBool();
        match(Lexer::Token::RBRACKET);

        return new ArrayAccessNode(lexer, indexExpr, id);
    }

    return new IdNode(lexer, id);
}

// <bool> -> <bool> || <join> | <join>
Node *Parser::parseBool() {
    Node *left = parseJoin();

    while (currentToken == Lexer::Token::OR) {
        match(Lexer::Token::OR);
        Node *right = parseJoin();
        left = new OrNode(lexer, left, right);
    }

    return left;
}

// <join> -> <join> && <equality> | <equality>
Node *Parser::parseJoin() {
    Node *left = parseEquality();

    while (currentToken == Lexer::Token::AND) {
        match(Lexer::Token::AND);
        Node *right = parseEquality();
        left = new AndNode(lexer, left, right);
    }

    return left;
}

// <equality> -> <equality> == <rel> | <equality> != <rel> | <rel>
Node *Parser::parseEquality() {
    Node *left = parseRel();

    while (currentToken == Lexer::Token::EQ || currentToken == Lexer::Token::NEQ) {
        bool isEqual = (currentToken == Lexer::Token::EQ);
        match(currentToken);
        Node *right = parseRel();
        left = new EqualityNode(lexer, left, right, isEqual);
    }

    return left;
}

// <rel> → <expr> < <expr> | <expr> <= <expr> | <expr> >= <expr> | <expr> > <expr> | <expr>
Node *Parser::parseRel() {
    Node *left = parseExpr();
    RelNode::Op op;

    // Lambda utility function
    auto getRelNode = [&](Lexer::Token currentToken, Node *left, RelNode::Op op) {
        match(currentToken);
        Node *right = parseExpr();

        return new RelNode(lexer, left, right, op);
    };

    switch (currentToken) {
        case Lexer::Token::LESS: return getRelNode(currentToken, left, RelNode::LESS);
        case Lexer::Token::LESSEQ: return getRelNode(currentToken, left, RelNode::LESSEQ);
        case Lexer::Token::GREATER: return getRelNode(currentToken, left, RelNode::GREATER);
        case Lexer::Token::GREATEREQ: return getRelNode(currentToken, left, RelNode::GREATEREQ);
        default: return left;
    }
}

// <expr> -> <expr> + <term> | <expr> - <term> | <term>
Node *Parser::parseExpr() {
    Node *left = parseTerm();

    while (currentToken == Lexer::Token::PLUS || currentToken == Lexer::Token::MINUS) {
        bool isAddition = (currentToken == Lexer::Token::PLUS);
        match(currentToken);
        Node *right = parseTerm();
        left = new AddNode(lexer, left, right, isAddition);
    }

    return left;
}

// <term> -> <term> * <unary> | <term> / <unary> | <unary>
Node *Parser::parseTerm() {
    Node *left = parseUnary();

    while (currentToken == Lexer::Token::MULTIPLY || currentToken == Lexer::Token::DIVIDE) {
        bool isMultiplication = (currentToken == Lexer::Token::MULTIPLY);
        match(currentToken);
        Node *right = parseUnary();
        left = new MulNode(lexer, left, right, isMultiplication);
    }

    return left;
}

// <unary> -> ! <unary> | - <unary> | <factor>
Node *Parser::parseUnary() {
    if (currentToken == Lexer::Token::NOT) {
        match(Lexer::Token::NOT);
        Node *operand = parseUnary();

        return new UnaryNode(lexer, operand, UnaryNode::NOT);
    } else if (currentToken == Lexer::Token::MINUS) {
        match(Lexer::Token::MINUS);
        Node *operand = parseUnary();

        return new UnaryNode(lexer, operand, UnaryNode::NEG);
    } else {
        return parseFactor();
    }
}

// <factor> -> ( <bool> ) | <loc> | num | true | false
Node *Parser::parseFactor() {
    Node *expr;
    std::string id;
    int value;

    switch (currentToken) {
        case Lexer::Token::LPARENTHESIS:
            match(Lexer::Token::LPARENTHESIS);
            expr = parseBool();
            match(Lexer::Token::RPARENTHESIS);

            return expr;
        case Lexer::Token::ID:
            id = lexer.getIdentifier();

            match(Lexer::Token::ID);
            if (currentToken == Lexer::Token::LBRACKET) { // Array access
                match(Lexer::Token::LBRACKET);
                expr = parseBool();
                match(Lexer::Token::RBRACKET);

                return new ArrayAccessNode(lexer, expr, id);
            } else {
                return new IdNode(lexer, id);
            }
        case Lexer::Token::NUM:
            value = lexer.getNumber();
            match(Lexer::Token::NUM);

            return new FactorNode(lexer, FactorNode::INT, value, false, nullptr);
        case Lexer::Token::TRUE:
            match(Lexer::Token::TRUE);

            return new FactorNode(lexer, FactorNode::BOOL, 0, true, nullptr);
        case Lexer::Token::FALSE:
            match(Lexer::Token::FALSE);

            return new FactorNode(lexer, FactorNode::BOOL, 0, false, nullptr);
        default:
            std::string errMsg = "Error: Unexpected token " + to_string(currentToken) + " at " + to_string(lexer);
            throw std::runtime_error(errMsg);
    }
}
