// File created by fob

#include "../include/interpreter.h"

// Returns the size of the array if it is an array
int Variable::size() const {
    if (!isArray) {
        throw std::runtime_error("Error: Variable is not an array");
    } else {
        return (int) std::max(intArray.size(), boolArray.size());         // Returns the size of either the intArray or boolArray
    }
}

// Checks if a variable with the specified name is declared in the map
bool SymbolMap::isDeclared(const std::string &name) const {
    return map.find(name) != map.end();
}

// Declares a new variable with the specified name, type, and array properties if it is an array
void SymbolMap::declareVariable(const std::string &name, Type type, bool isArray, int arraySize) {
    if (isDeclared(name)) {
        map.erase(name); // Removes the old variable if it exists
    }

    Variable variable;

    variable.type = type;
    variable.isArray = isArray;

    if (isArray) {
        // Initializes arrays based on the variable type
        if (type == Type::INT) {
            variable.intArray.resize(arraySize, 0);
        } else {
            variable.boolArray.resize(arraySize, false);
        }
        variable.arrayInitialized.resize(arraySize, false); // Tracking for variable in array initialized
    }

    map.insert({name, variable});
}

// Retrieves a variable by name
Variable &SymbolMap::getVariable(const std::string &name) {
    if (!isDeclared(name)) {
        std::string errMsg = "Error: Variable " + name + " not initialized";
        throw std::runtime_error(errMsg);
    }

    return map[name];
}

// Throws a runtime error with the provided error message specifying line and colum
void Interpreter::throwError(const std::string &message, Node *node) {
    std::string errMsg = "Error: " + message + " at line: " + std::to_string(node->line) + " column: " + std::to_string(node->column) + " type " + typeid(node).name();
    throw std::runtime_error(errMsg);
}

// Assigns a value to a variable or array element
void Interpreter::assignValue(Node *locNode, Node *exprNode) {
    if (auto *idNode = dynamic_cast<IdNode *>(locNode)) {
        Variable &variable = symbolMap.getVariable(idNode->id);
        Result value = evaluateExpr(exprNode);

        // Checks if types match between the variable and the expression result
        if (variable.type == value.type) {
            if (variable.type == Type::INT && !variable.isArray) {
                variable.intValue = value.value;
            } else if (variable.type == Type::BOOL && !variable.isArray) {
                variable.boolValue = (bool) value.value;
            } else {
                throwError("Invalid assignment", idNode);
            }
        } else {
            throwError("Value mismatch", idNode);
        }

        variable.initialized = true;
    } else if (auto *arrayAccessNode = dynamic_cast<ArrayAccessNode *>(locNode)) {
        Variable &variable = symbolMap.getVariable(arrayAccessNode->id);
        Result index = evaluateExpr(arrayAccessNode->index);
        Result value = evaluateExpr(exprNode);

        // Check the array index bounds
        int idx = index.type == Type::INT ? index.value : (bool) index.value;
        if (idx < 0 || idx >= variable.size()) {
            throwError("Array index out of bounds 0<=" + std::to_string(idx) + "<" + std::to_string(variable.size()) , arrayAccessNode);
        } else {
            // Ensures the value type matches
            if (variable.type == value.type) {
                if (variable.type == Type::INT && variable.isArray) {
                    variable.intArray[idx] = value.value;
                } else if (variable.type == Type::BOOL && variable.isArray) {
                    variable.boolArray[idx] = (bool) value.value;
                } else {
                    throwError("Invalid array assignment", arrayAccessNode);
                }
            } else {
                throwError("Value mismatch", arrayAccessNode);
            }
        }

        variable.arrayInitialized.insert(variable.arrayInitialized.begin() + index.value, true);
    } else {
        throwError("Invalid location node in assignment", locNode);
    }
}

// Interpret the root program node
void Interpreter::interpret(Node* node) {
    if (auto *programNode = dynamic_cast<ProgramNode *>(node)) {
        executeBlock(programNode->block);
    } else {
        throwError("Program should start with a ProgramNode", programNode);
    }
}

// Executes a block node
void Interpreter::executeBlock(Node *blockNode) {
    if (auto *block = dynamic_cast<BlockNode *>(blockNode)) {
        if (Node *decls = block->decls) {
            executeDecls(decls);
        }

        if (Node *stmts = block->stmts) {
            executeStmts(stmts);
        }
    } else {
        throwError("Invalid block node", blockNode);
    }
}

// Executes a sequence of declarations
void Interpreter::executeDecls(Node *declsNode) {
    if (auto *decls = dynamic_cast<DeclsNode *>(declsNode)) {
        executeDecl(decls->decl); // Run the current declaration

        if (Node *next = decls->next) {
            executeDecls(next);
        }
    } else {
        throwError("Invalid declarations node", declsNode);
    }
}

// Executes a single declaration
void Interpreter::executeDecl(Node *declNode) {
    if (auto *decl = dynamic_cast<DeclNode *>(declNode)) {
        // Basic types
        if (auto *basicType = dynamic_cast<BasicTypeNode *>(decl->type)) {
            if (basicType->typeName == "integer") {
                symbolMap.declareVariable(decl->id, Type::INT);
            } else if (basicType->typeName == "boolean") {
                symbolMap.declareVariable(decl->id, Type::BOOL);
            } else {
                throwError("Unknown basic type " + basicType->typeName, basicType);
            }
        // Array Types
        } else if (auto *arrayType = dynamic_cast<ArrayTypeNode *>(decl->type)) {
            if (auto *baseType = dynamic_cast<BasicTypeNode *>(arrayType->type)) {
                if (baseType->typeName == "integer") {
                    symbolMap.declareVariable(decl->id, Type::INT, true, arrayType->arraySize);
                } else if (baseType->typeName == "boolean") {
                    symbolMap.declareVariable(decl->id, Type::BOOL, true, arrayType->arraySize);
                } else {
                    throwError("Unknown array base type " + baseType->typeName, baseType);
                }
            } else {
                throwError("Invalid array type", arrayType);
            }
        } else {
            throwError("Invalid type node in declaration", decl);
        }
    } else {
        throwError("Invalid declaration node", declNode);
    }
}

// Executes a sequence of statements
void Interpreter::executeStmts(Node *stmtsNode) {
    if (auto *stmts = dynamic_cast<StmtsNode *>(stmtsNode)) {
        executeStmt(stmts->stmt); // Run the current statement

        if (Node *next = stmts->next) {
            executeStmts(next);
        }
    } else {
        throwError("Invalid statements node", stmtsNode);
    }
}

// Executes a single statement
void Interpreter::executeStmt(Node *stmtNode) {
    // Assign
    if (auto *assign = dynamic_cast<AssignNode *>(stmtNode)) {
        assignValue(assign->loc, assign->expr);
    // If
    } else if (auto *ifStmt = dynamic_cast<IfNode *>(stmtNode)) {
        if ((bool) evaluateExpr(ifStmt->condition).value) {
            executeStmt(ifStmt->ifStmt);
        }
    // If Else
    } else if (auto *ifElseStmt = dynamic_cast<IfElseNode *>(stmtNode)) {
        if ((bool) evaluateExpr(ifElseStmt->condition).value) {
            executeStmt(ifElseStmt->ifStmt);
        } else {
            executeStmt(ifElseStmt->elseStmt);
        }
    // While
    } else if (auto *whileStmt = dynamic_cast<WhileNode *>(stmtNode)) {
        try {
            while ((bool) evaluateExpr(whileStmt->condition).value) {
                executeStmt(whileStmt->body);
            }
        } catch (const BreakException &) {
            // Exit from the cycle
        }
    // Do While
    } else if (auto *doWhileStmt = dynamic_cast<DoWhileNode *>(stmtNode)) {
        try {
            do {
                executeStmt(doWhileStmt->body);
            } while ((bool) evaluateExpr(doWhileStmt->condition).value);
        } catch (const BreakException &) {
            // Exit from the cycle
        }
    // Print
    } else if (auto *printStmt = dynamic_cast<PrintNode *>(stmtNode)) {
        Result result = evaluateExpr(printStmt->expr);

        if (result.type == Type::INT) {
            std::cout << result.value << std::endl;
        } else if (result.type == Type::BOOL) {
            std::cout << ((bool) result.value ? "true" : "false") << std::endl;
        }
    // Break
    } else if (auto *_ = dynamic_cast<BreakNode *>(stmtNode)) {
        throw BreakException();
    } else if (auto *blockStmt = dynamic_cast<BlockNode *>(stmtNode)) {
        executeBlock(blockStmt);
    } else {
        throwError("Unknown statement type", stmtNode);
    }
}

// Evaluates an expression
Result Interpreter::evaluateExpr(Node *exprNode) {
    // Multiplication
    if (auto *mulNode = dynamic_cast<MulNode *>(exprNode)) {
        Result leftValue = evaluateExpr(mulNode->left);
        Result rightValue = evaluateExpr(mulNode->right);

        if (!mulNode->isMultiplication && (rightValue.type == Type::INT && rightValue.value == 0 || rightValue.type == Type::BOOL && !rightValue.value)) {
            throwError("Impossible dividing by 0", mulNode);
        }

        if (leftValue.type != rightValue.type) {
            throwError("Value type mismatch", mulNode);
        }

        if (leftValue.type == Type::INT) {
            return Result(mulNode->isMultiplication ? leftValue.value * rightValue.value : leftValue.value / rightValue.value);
        } else {
            return Result(mulNode->isMultiplication ? (bool) ((bool) leftValue.value * (bool) rightValue.value) : (bool) leftValue.value);
        }
    // Addition
    } else if (auto *addNode = dynamic_cast<AddNode *>(exprNode)) {
        Result leftValue = evaluateExpr(addNode->left);
        Result rightValue = evaluateExpr(addNode->right);

        if (leftValue.type != rightValue.type) {
            throwError("Value type mismatch", addNode);
        }

        if (leftValue.type == Type::INT) {
            return Result(addNode->isAddition ? leftValue.value + rightValue.value : leftValue.value - rightValue.value);
        } else {
            return Result(addNode->isAddition ? (bool) ((bool) leftValue.value + (bool) rightValue.value) : (bool) ((bool) leftValue.value - (bool) rightValue.value));
        }
    // Unary operation
    } else if (auto *unaryNode = dynamic_cast<UnaryNode *>(exprNode)) {
        Result operand = evaluateExpr(unaryNode->operand);

        // Not
        if (unaryNode->op == UnaryNode::Op::NOT && operand.type == Type::BOOL) {
            return Result(!(bool) operand.value);
        // Minus
        } else if (unaryNode->op == UnaryNode::Op::NEG && operand.type == Type::INT) {
            return Result(-operand.value);
        } else {
            throwError("Mismatched unary operation type", unaryNode);
        }
    // Factor
    } else if (auto *factorNode = dynamic_cast<FactorNode *>(exprNode)) {
        switch (factorNode->type) {
            case FactorNode::BOOL: return Result(factorNode->boolValue);
            case FactorNode::INT: return Result(factorNode->intValue);
            case FactorNode::ID: return evaluateExpr(factorNode->loc);
        }
    // Or
    } else if (auto *orNode = dynamic_cast<OrNode *>(exprNode)) {
        return Result((bool) evaluateExpr(orNode->left).value || (bool) evaluateExpr(orNode->right).value);
    // And
    } else if (auto *andNode = dynamic_cast<AndNode *>(exprNode)) {
        return Result((bool) evaluateExpr(andNode->left).value && (bool) evaluateExpr(andNode->right).value);
    // Equality
    } else if (auto *eqNode = dynamic_cast<EqualityNode *>(exprNode)) {
        Result leftValue = evaluateExpr(eqNode->left);
        Result rightValue = evaluateExpr(eqNode->right);

        if (leftValue.type != rightValue.type) {
            throwError("Value type mismatch", addNode);
        }

        if (leftValue.type == Type::INT) {
            return Result(eqNode->isEqual == (leftValue.value == rightValue.value));
        } else {
            return Result(eqNode->isEqual == ((bool) leftValue.value == (bool) rightValue.value));
        }
    // Relation
    } else if (auto *relNode = dynamic_cast<RelNode *>(exprNode)) {
        Result left = evaluateExpr(relNode->left);
        Result right = evaluateExpr(relNode->right);

        switch (relNode->op) {
            case RelNode::LESS: return Result(left.value < right.value);
            case RelNode::LESSEQ: return Result(left.value <= right.value);
            case RelNode::GREATER: return Result(left.value > right.value);
            case RelNode::GREATEREQ: return Result(left.value >= right.value);
        }
    // Id
    } else if (auto *idNode = dynamic_cast<IdNode *>(exprNode)) {
        Variable &variable = symbolMap.getVariable(idNode->id);

        if (!variable.initialized) {
            throwError("Variable" + idNode->id + " not initialized yet", idNode);
        }

        return variable.type == Type::INT ? Result(variable.intValue) : Result(variable.boolValue);
    // Array access
    } else if (auto *arrayAccessNode = dynamic_cast<ArrayAccessNode *>(exprNode)) {
        Variable &variable = symbolMap.getVariable(arrayAccessNode->id);
        int index = evaluateExpr(arrayAccessNode->index).value;

        if (!variable.arrayInitialized[index]) {
            throwError("Array " + arrayAccessNode->id + " value at " + std::to_string(index) + " not initialized yet", arrayAccessNode);
        }

        return variable.type == Type::INT ? Result(variable.intArray[index]) : Result(variable.boolArray[index]);
    } else {
        throwError("Node interpretation not implemented yet", exprNode);
    }

    return Result(0); // Default value ( Should never be called )
}