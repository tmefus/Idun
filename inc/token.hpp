#pragma once

#include <memory>
#include <unordered_map>
#include "value.hpp"

enum class TokenType {
    LEFT_PAREN, RIGHT_PAREN, LEFT_SQUARE, RIGHT_SQUARE, LEFT_BRACE, RIGHT_BRACE,    // ( ) [ ] { }
    PLUS, MINUS, STAR, SLASH, MOD, POWER,                                           // + - * / % ** ${
    COMMA, DOT, COLON, SEMICOLON, NEWLINE, RANGE, ARROW,                            // , . : ; '\n' .. ->
    PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MOD_EQUAL,                    // += -= *= /= %=

    EQUAL, NOT_EQUAL, EQUAL_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,        // = != == < <= > >=
    AND, OR, NOT, BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, SHIFT_L, SHIFT_R, SHIFT_RA,    // && || ! & | ^ ~ << >>

    STR_START, STR_END,                                                             // 字符串的开始、结束

    BREAK, CLASS, CONTINUE, ELSE, ELIF, ENUM, FALSE, FUN, FOR, IF, IMPORT,
    IN, IS, LET, NIL, NOTIN, NOTIS, RETURN, SUPER, THIS, TRUE, VAR, WHEN, WHILE,

    IDENTIFIER, INTEGER, FLOATING, STRING, ENDMARKER
};

static const char *getTokenTypeStr(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::LEFT_PAREN:return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:return "RIGHT_PAREN";
        case TokenType::LEFT_SQUARE:return "LEFT_SQUARE";
        case TokenType::RIGHT_SQUARE:return "RIGHT_SQUARE";
        case TokenType::LEFT_BRACE:return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:return "RIGHT_BRACE";
        case TokenType::PLUS:return "PLUS";
        case TokenType::MINUS:return "MINUS";
        case TokenType::STAR:return "STAR";
        case TokenType::SLASH:return "SLASH";
        case TokenType::MOD:return "MOD";
        case TokenType::POWER:return "POWER";
        case TokenType::COMMA:return "COMMA";
        case TokenType::DOT:return "DOT";
        case TokenType::COLON:return "COLON";
        case TokenType::SEMICOLON:return "SEMICOLON";
        case TokenType::NEWLINE:return "NEWLINE";
        case TokenType::RANGE:return "RANGE";
        case TokenType::ARROW:return "ARROW";
        case TokenType::PLUS_EQUAL:return "PLUS_EQUAL";
        case TokenType::MINUS_EQUAL:return "MINUS_EQUAL";
        case TokenType::STAR_EQUAL:return "STAR_EQUAL";
        case TokenType::SLASH_EQUAL:return "SLASH_EQUAL";
        case TokenType::MOD_EQUAL:return "MOD_EQUAL";
        case TokenType::EQUAL:return "EQUAL";
        case TokenType::NOT_EQUAL:return "NOT_EQUAL";
        case TokenType::EQUAL_EQUAL:return "EQUAL_EQUAL";
        case TokenType::LESS:return "LESS";
        case TokenType::LESS_EQUAL:return "LESS_EQUAL";
        case TokenType::GREATER:return "GREATER";
        case TokenType::GREATER_EQUAL:return "GREATER_EQUAL";
        case TokenType::AND:return "AND";
        case TokenType::OR:return "OR";
        case TokenType::NOT:return "NOT";
        case TokenType::BIT_AND:return "BIT_AND";
        case TokenType::BIT_OR:return "BIT_OR";
        case TokenType::BIT_XOR:return "BIT_XOR";
        case TokenType::BIT_NOT:return "BIT_NOT";
        case TokenType::SHIFT_L:return "SHIFT_L";
        case TokenType::SHIFT_R:return "SHIFT_R";
        case TokenType::SHIFT_RA:return "SHIFT_RA";
        case TokenType::STR_START:return "STR_START";
        case TokenType::STR_END:return "STR_END";
        case TokenType::BREAK:return "BREAK";
        case TokenType::CLASS:return "CLASS";
        case TokenType::CONTINUE:return "CONTINUE";
        case TokenType::ELSE:return "ELSE";
        case TokenType::ELIF:return "ELIF";
        case TokenType::ENUM:return "ENUM";
        case TokenType::FALSE:return "FALSE";
        case TokenType::FUN:return "FUN";
        case TokenType::FOR:return "FOR";
        case TokenType::IF:return "IF";
        case TokenType::IMPORT:return "IMPORT";
        case TokenType::IN:return "IN";
        case TokenType::IS:return "IS";
        case TokenType::LET:return "LET";
        case TokenType::NIL:return "NIL";
        case TokenType::RETURN:return "RETURN";
        case TokenType::SUPER:return "SUPER";
        case TokenType::THIS:return "THIS";
        case TokenType::TRUE:return "TRUE";
        case TokenType::VAR:return "VAR";
        case TokenType::WHEN:return "WHEN";
        case TokenType::WHILE:return "WHILE";
        case TokenType::IDENTIFIER:return "IDENTIFIER";
        case TokenType::INTEGER:return "INTEGER";
        case TokenType::FLOATING:return "FLOATING";
        case TokenType::STRING:return "STRING";
        case TokenType::ENDMARKER:return "ENDMARKER";
        default:return "UNKNOWN";
    }
}

static const std::unordered_map<std::string, TokenType> keywords = {
        {"and",      TokenType::AND},
        {"or",       TokenType::OR},
        {"not",      TokenType::NOT},
        {"break",    TokenType::BREAK},
        {"break",    TokenType::BREAK},
        {"break",    TokenType::BREAK},
        {"class",    TokenType::CLASS},
        {"continue", TokenType::CONTINUE},
        {"else",     TokenType::ELSE},
        {"elif",     TokenType::ELIF},
        {"enum",     TokenType::ENUM},
        {"false",    TokenType::FALSE},
        {"fun",      TokenType::FUN},
        {"for",      TokenType::FOR},
        {"if",       TokenType::IF},
        {"import",   TokenType::IMPORT},
        {"in",       TokenType::IN},
        {"is",       TokenType::IS},
        {"let",      TokenType::LET},
        {"nil",      TokenType::NIL},
        {"return",   TokenType::RETURN},
        {"super",    TokenType::SUPER},
        {"this",     TokenType::THIS},
        {"true",     TokenType::TRUE},
        {"var",      TokenType::VAR},
        {"when",     TokenType::WHEN},
        {"while",    TokenType::WHILE}
};

struct Token {
    std::string lexeme;
    LoxValuePtr value;
    TokenType type;
    int line;

    Token(TokenType tokenType, LoxValuePtr value, std::string lexeme, int line)
            : type{tokenType}, value{std::move(value)}, lexeme{std::move(lexeme)}, line{line} {}

    friend std::ostream &operator<<(std::ostream &out, Token &token) {
        out << '(' << token.line << ") " << '[' << getTokenTypeStr(token.type);

        if (token.type != TokenType::ENDMARKER) {
            out << " -> " << token.lexeme;
        }

        out << ']';

        if (token.type == TokenType::STRING || token.type == TokenType::INTEGER || token.type == TokenType::FLOATING) {
            out << " (value=" << *(token.value) << ')';
        }

        return out;
    }
};

using TokenPtr = std::shared_ptr<Token>;
