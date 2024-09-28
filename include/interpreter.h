// File created by fob

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// Enum representing the supported data types in the language (integer and boolean)
enum class Type { INT, BOOL };

// Struct to represent the result of evaluating an expression
// Stores both the type and the value of the result
struct Result {
    Type type;      // The type of the result (INT or BOOL)
    int value;      // The value, represented as an integer (for both int and bool types)

    // Constructor for integer results
    Result(int value) : type(Type::INT), value(value) {}

    // Constructor for boolean results
    Result(bool value) : type(Type::BOOL), value(value) {}
};

// Class representing a variable in the program
// Can store either a single value or an array of values
class Variable {
public:
    Type type;                      // The data type of the variable (INT or BOOL)
    int intValue;                   // Single integer value (if not an array)
    bool boolValue;                 // Single boolean value (if not an array)
    bool isArray;                   // Flag indicating whether the variable is an array
    std::vector<int> intArray;      // Array of integer values (if the variable is an array)
    std::vector<bool> boolArray;    // Array of boolean values (if the variable is an array)

    bool initialized;                   // Flag indicating if the single value is initialized
    std::vector<bool> arrayInitialized; // Flags for initialization of each array element (if it's an array)

    // Default constructor
    Variable() : isArray(false), intValue(0), boolValue(false) {}

    // Returns the size of the array if it is an array
    int size() const;
};

// Class representing the symbol table (or variable map) for the program
// It stores variables and supports variable declarations and lookups
class SymbolMap {
public:
    // Checks if a variable with a given name is already declared
    bool isDeclared(const std::string &name) const;

    // Declares a new variable with a given name, type, and array size (optional)
    void declareVariable(const std::string &name, Type type, bool isArray = false, int arraySize = 0);

    // Returns a reference to the Variable object for a given variable name
    Variable &getVariable(const std::string &name);

private:
    // Hashmap to store the declared variables by name
    std::unordered_map<std::string, Variable> map;
};


// Class representing the interpreter that executes the abstract syntax tree (AST)
class Interpreter {
public:
    // Exception class for handling break statements in loops
    class BreakException : public std::exception {
    public:
        // Custom message for the break exception
        const char* what() const noexcept override {
            return "Break statement encountered";
        }
    };

    // Main function to interpret (execute) a given AST node
    void interpret(Node *node);

private:
    // Symbol table for managing declared variables during interpretation
    SymbolMap symbolMap;

    // Helper functions for interpreting different parts of the AST
    void executeBlock(Node *node);    // Interprets a block of code (e.g., inside a function or a loop)
    void executeDecls(Node *node);    // Interprets variable declarations
    void executeDecl(Node *node);     // Interprets a single variable declaration
    void executeStmts(Node *node);    // Interprets a list of statements
    void executeStmt(Node *node);     // Interprets a single statement

    // Evaluates an expression node and returns the resulting value
    Result evaluateExpr(Node *node);

    // Utility function to assign a value to a variable location
    void assignValue(Node *locNode, Node *node);

    // Throws a runtime error with a specific message related to a node
    static void throwError(const std::string &message, Node *node);
};

#endif // INTERPRETER_H