#pragma once

#include <utility>
#include <vector>
#include <optional>

#include "token.hpp"


class Scanner {
private:
    std::string program;
    std::vector<TokenPtr> tokens;
    int current{0}, start{0}, line{1};
    bool hasError{false};

public:
    explicit Scanner(std::string program) : program{std::move(program)} {};

    std::optional<std::vector<TokenPtr>> getTokens();

private:
    bool atEnd() {
        return (current >= program.length());
    };

    char advance() {
        return program[current++];
    };

    char peek() {
        if (atEnd()) return '\0';
        return program[current];
    };

    char peekNext() {
        if (current + 1 >= program.length()) return '\0';
        return program[current + 1];
    };

    bool match(char c) {
        if (atEnd()) return false;
        if (peek() == c) {
            advance();
            return true;
        }
        return false;
    };

    void addToken(TokenType tokenType, const LoxValuePtr &value = nullptr) {
        std::string lexeme = program.substr(start, current - start);
        tokens.emplace_back(std::make_shared<Token>(tokenType, value, lexeme, line));
    }

    static bool isNum(char c) {
        return (c >= '0' and c <= '9');
    };

    static bool isAlpha(char c) {
        return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or c == '_';
    }

    static bool isAlphaNum(char c) {
        return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or c == '_' or (c >= '0' and c <= '9');
    }

    void parseString();

    void parseNumber();

    void parseIdentifier();

    void parseComments();

    void fillStrRefers(const std::string &str);
};