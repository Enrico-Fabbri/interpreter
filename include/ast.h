// File created by fob
#ifndef AST_H
#define AST_H

#include "lexer.h"

#include <string>
#include <iostream>

// The AST is the abstract syntax tree

// Base class for syntax tree nodes
class Node {
    public:
        // Position variables
        int line;
        int column;

        // Constructor
        Node(Lexer &lexer) {
            line = lexer.getLine();
            column = lexer.getColumn();
        }

        virtual ~Node() = default; // Destructor

        // Virtual function to override the printing for each node
        virtual void print(std::ostream& out, int indent = 0) const = 0;
};

// Program Node
class ProgramNode : public Node {
    public:
        Node* block; // Block

        ProgramNode(Lexer &lexer, Node* block) : Node(lexer), block(block) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "ProgramNode\n";
            block->print(out, indent + 2);
        }
};

// Block Node
class BlockNode : public Node {
    public:
        // Child nodes
        Node* decls; // Declarations
        Node* stmts; // Statements

        BlockNode(Lexer &lexer, Node* decls, Node* stmts) : Node(lexer), decls(decls), stmts(stmts) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "BlockNode\n";
            if (decls) decls->print(out, indent + 2);
            if (stmts) stmts->print(out, indent + 2);
        }
};

// Declarations Node
class DeclsNode : public Node {
    public:
        // Child nodes
        Node* decl; // Declaration
        Node* next; // Next declarations

        DeclsNode(Lexer &lexer, Node* decl, Node* next) : Node(lexer), decl(decl), next(next) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "DeclsNode\n";
            if (decl) decl->print(out, indent + 2);
            if (next) next->print(out, indent + 2);
        }
};

// Statements Node
class StmtsNode : public Node {
    public:
        // Child nodes
        Node* stmt; // Statement
        Node* next; // Next statements

        StmtsNode(Lexer &lexer, Node* stmt, Node* next) : Node(lexer), stmt(stmt), next(next) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "StmtsNode\n";
            if (stmt) stmt->print(out, indent + 2);
            if (next) next->print(out, indent + 2);
        }
};

// Declaration Node
class DeclNode : public Node {
    public:
        // Child nodes
        Node* type;       // Basic Type or Array Type
        std::string id;   // Identifier

        DeclNode(Lexer &lexer, Node* type, std::string id) : Node(lexer), type(type), id(id) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "DeclNode: " << id << "\n";
            type->print(out, indent + 2);
        }
};

// Basic Type Node
class BasicTypeNode : public Node {
    public:
        std::string typeName; // Type name

        BasicTypeNode(Lexer &lexer, std::string typeName) : Node(lexer), typeName(typeName) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "BasicTypeNode: " << typeName << "\n";
        }
};

// Array Type Node
class ArrayTypeNode : public Node {
    public:
        Node* type;     // Basic type
        int arraySize;  // Array size

        ArrayTypeNode(Lexer &lexer, Node* type, int arraySize) : Node(lexer), type(type), arraySize(arraySize) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "ArrayTypeNode: size = " << arraySize << "\n";
            type->print(out, indent + 2);
        }
};

// Identifier Node
class IdNode : public Node {
    public:
        std::string id; // Identifier

        IdNode(Lexer &lexer, std::string id) : Node(lexer), id(id) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "IdNode: " << id << "\n";
        }
};

// Assignment Node
class AssignNode : public Node {
    public:
        // Child nodes
        Node* loc;      // Locator
        Node* expr;     // Expression

        AssignNode(Lexer &lexer, Node* loc, Node* expr) : Node(lexer), loc(loc), expr(expr) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "AssignNode\n";
            loc->print(out, indent + 2);
            expr->print(out, indent + 2);
        }
};

// Array Access Node
class ArrayAccessNode : public Node {
    public:
        Node* index;    // Expression
        std::string id; // Identifier

        ArrayAccessNode(Lexer &lexer, Node* index, std::string id) : Node(lexer), index(index), id(id) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "ArrayAccessNode: " << id << "\n";
            index->print(out, indent + 2);
        }
};

// Or Node
class OrNode : public Node {
    public:
        // Child nodes
        Node* left;  // Left operand
        Node* right; // Right operand

        OrNode(Lexer &lexer, Node* left, Node* right) : Node(lexer), left(left), right(right) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "OrNode\n";
            left->print(out, indent + 2);
            right->print(out, indent + 2);
        }
};

// And Node
class AndNode : public Node {
    public:
        // Child nodes
        Node* left;  // Left operand
        Node* right; // Right operand

        AndNode(Lexer &lexer, Node* left, Node* right) : Node(lexer), left(left), right(right) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "AndNode\n";
            left->print(out, indent + 2);
            right->print(out, indent + 2);
        }
};

// Equality Node (== and !=)
class EqualityNode : public Node {
    public:
        // Child nodes
        Node* left;    // Left operand
        Node* right;   // Right operand
        bool isEqual;  // Type identifier (true for '==' and false for '!=')

        EqualityNode(Lexer &lexer, Node* left, Node* right, bool isEqual) : Node(lexer), left(left), right(right), isEqual(isEqual) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "EqualityNode: " << (isEqual ? "==" : "!=") << "\n";
            left->print(out, indent + 2);
            right->print(out, indent + 2);
        }
};

// Relational Node (<, <=, >, >=)
class RelNode : public Node {
    public:
        enum Op { LESS, LESSEQ, GREATER, GREATEREQ }; // Operations
        // Child nodes
        Node* left;     // Left operand
        Node* right;    // Right operand
        Op op;          // Operation type

        RelNode(Lexer &lexer, Node* left, Node* right, Op op) : Node(lexer), left(left), right(right), op(op) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            static const char* opNames[] = { "<", "<=", ">", ">=" };
            out << std::string(indent, ' ') << "RelNode: " << opNames[op] << "\n";
            left->print(out, indent + 2);
            right->print(out, indent + 2);
        }
};

// Addition and Subtraction Node (+, -)
class AddNode : public Node {
    public:
        // Child nodes
        Node* left;         // Left operand
        Node* right;        // Right operand
        bool isAddition;    // Type identifier (true for '+' false for '-')

        AddNode(Lexer &lexer, Node* left, Node* right, bool isAddition) : Node(lexer), left(left), right(right), isAddition(isAddition) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "AddNode: " << (isAddition ? "+" : "-") << "\n";
            left->print(out, indent + 2);
            right->print(out, indent + 2);
        }
};

// Multiplication and Division Node (*, /)
class MulNode : public Node {
    public:
        // Child nodes
        Node* left;             // Left operand
        Node* right;            // Right operand
        bool isMultiplication;  // Type identifier (true for '*' false for '/')

        MulNode(Lexer &lexer, Node* left, Node* right, bool isMultiplication) : Node(lexer), left(left), right(right), isMultiplication(isMultiplication) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "MulNode: " << (isMultiplication ? "*" : "/") << "\n";
            left->print(out, indent + 2);
            right->print(out, indent + 2);
        }
};

// Unary Operations Node (!, -)
class UnaryNode : public Node {
    public:
        enum Op { NOT, NEG }; // Operations
        Node* operand;        // Operand
        Op op;                // Operation type

        UnaryNode(Lexer &lexer, Node* operand, Op op) : Node(lexer), operand(operand), op(op) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "UnaryNode: " << (op == NOT ? "!" : "-") << "\n";
            operand->print(out, indent + 2);
        }
};

// Factor Node (INT, BOOL, ID, LOC)
class FactorNode : public Node {
    public:
        enum Type { BOOL, INT, ID }; // Types
        Node* loc;          // Locator node
        int intValue;       // Value if INT
        bool boolValue;     // Value if BOOL
        Type type;          // Type

        FactorNode(Lexer &lexer, Type type, int intValue = 0, bool boolValue = false, Node* loc = nullptr)
            : Node(lexer), type(type), intValue(intValue), boolValue(boolValue), loc(loc) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "FactorNode: ";
            switch(type) {
                case BOOL: out << "bool = " << (boolValue ? "true" : "false") << "\n"; break;
                case INT: out << "int = " << intValue << "\n"; break;
                case ID: loc->print(out, indent + 2); break;
            }
        }
};

// If Node
class IfNode : public Node {
    public:
        // Child nodes
        Node* condition; // Condition
        Node* ifStmt;    // If statement

        IfNode(Lexer &lexer, Node* condition, Node* ifStmt) : Node(lexer), condition(condition), ifStmt(ifStmt) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "IfNode\n";
            condition->print(out, indent + 2);
            ifStmt->print(out, indent + 2);
        }
};

// If Else Node
class IfElseNode : public Node {
    public:
        // Child nodes
        Node* condition; // Condition
        Node* ifStmt;    // If statement
        Node* elseStmt;  // Else statement

        IfElseNode(Lexer &lexer, Node* condition, Node* ifStmt, Node* elseStmt) : Node(lexer), condition(condition), ifStmt(ifStmt), elseStmt(elseStmt) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "IfElseNode\n";
            condition->print(out, indent + 2);
            ifStmt->print(out, indent + 2);
            elseStmt->print(out, indent + 2);
        }
};

// While Node
class WhileNode : public Node {
    public:
        // Child nodes
        Node* condition; // Condition
        Node* body;      // Body

        WhileNode(Lexer &lexer, Node* condition, Node* body) : Node(lexer), condition(condition), body(body) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "WhileNode\n";
            condition->print(out, indent + 2);
            body->print(out, indent + 2);
        }
};

// Do While Node
class DoWhileNode : public Node {
    public:
        // Child nodes
        Node* condition; // Condition
        Node* body;      // Body

        DoWhileNode(Lexer &lexer, Node* body, Node* condition) : Node(lexer), body(body), condition(condition) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "DoWhileNode\n";
            body->print(out, indent + 2);
            condition->print(out, indent + 2);
        }
};

// Print Node
class PrintNode : public Node {
    public:
        Node* expr; // Expression

        PrintNode(Lexer &lexer, Node* expr) : Node(lexer), expr(expr) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "PrintNode\n";
            expr->print(out, indent + 2);
        }
};

// Break Node
class BreakNode : public Node {
    public:
        BreakNode(Lexer &lexer) : Node(lexer) {} // Constructor

        void print(std::ostream& out, int indent = 0) const override {
            out << std::string(indent, ' ') << "BreakNode\n";
        }
};

// Helper function to handle the printing of the AST
inline std::ostream& operator<<(std::ostream& out, const Node& node) {
    node.print(out);
    return out;
}

#endif // AST_H
