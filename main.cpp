// File created by fob

#include "include/interpreter.h"
#include "include/lexer.h"
#include "include/parser.h"

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::string errMsg = "Error: Expected 2 arguments found " + std::to_string(argc);
            throw std::runtime_error(errMsg);
        }

        std::ifstream file(argv[1]);

        if (!file.is_open()) {
            std::string errMsg = "Error: Unable to open file " + ((std::string) argv[1]);
            throw std::runtime_error(errMsg);
        }

        Lexer lexer(file);
        Parser parser(lexer);
        Interpreter interpreter;

        interpreter.interpret(parser.parse());

        std::cout.flush();
        file.close();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
