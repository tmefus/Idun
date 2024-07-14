#include <iostream>
#include <fstream>
#include "parser.hpp"
#include "resolver.hpp"
#include "interpreter.hpp"


void run(std::string &source) {
    // 词法解析
    Scanner scanner{source};
    auto tokens = scanner.getTokens();
    if (!tokens) return;

    // 语法解析
    Parser parser(tokens.value());
    auto ast = parser.parse();
    if (!ast) return;

    // 语义分析
    Interpreter interpreter;
    Resolver resolver{interpreter};
    bool resolve_result = resolver.resolve(ast.value());
    if (!resolve_result) return;

    // 解释执行
    interpreter.interpret(ast.value());
}

void runFromFile(const char *path) {
    std::ifstream file{path, std::ios_base::in | std::ios_base::binary};

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(-1);
    }

    std::string source;

    file.seekg(0, std::ios_base::end);
    source.resize(file.tellg());
    file.seekg(0, std::ios_base::beg);
    file.read(&source[0], (int) source.size());

    run(source);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [script]" << std::endl;
        return 1;
    } else {
        runFromFile(argv[1]);
    }
    return 0;
}
