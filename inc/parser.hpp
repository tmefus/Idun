#pragma once

#include "scanner.hpp"
#include "token.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "lox_exception.hpp"
#include <optional>
#include <utility>
#include <vector>
#include <queue>

class Parser {
public:
    explicit Parser(std::vector<TokenPtr> tokens) : tokens{std::move(tokens)} {};

    std::optional<std::vector<StmtPtr>> parse();

private:
    std::vector<TokenPtr> tokens;
    size_t current{0};
    bool parsing_failed{false};

    static parsing_error error(const TokenPtr &token, const std::string &msg) {
        if (token->type == TokenType::ENDMARKER) {
            std::cerr << "line " << token->line << " error at end: " << msg << std::endl;
        } else {
            std::cerr << "line " << token->line << " error at '" << token->lexeme << "': " << msg << std::endl;
        }
        return parsing_error{""};
    };

    // Helpers
    TokenPtr peek() { return tokens[current]; };

    TokenPtr previous() { return tokens[current - 1]; };

    bool atEnd() {
        return peek()->type == TokenType::ENDMARKER;
    };

    TokenPtr advance() {
        if (not atEnd()) ++current;
        return previous();
    };

    bool check(TokenType tokenType) {
        if (atEnd()) return false;
        return peek()->type == tokenType;
    };

    TokenPtr consume(TokenType tokenType, const std::string &msg) {
        if (check(tokenType)) return advance();
        throw error(peek(), msg);
    };

    bool match(TokenType tokenType) {
        if (check(tokenType)) {
            advance();
            return true;
        }
        return false;
    }

    template<typename... Args>
    bool match(TokenType tokenType, Args... args) {
        return match(tokenType) or match(args...);
    }

    // Parsers
    ExprPtr parseExpression();

    ExprPtr parseAssignment();

    ExprPtr parseOr();

    ExprPtr parseAnd();

    ExprPtr parseEquality();

    ExprPtr parseComparison();

    ExprPtr parseInIsExpr(bool needLeft = true);

    ExprPtr parseRangeExpr();

    ExprPtr parseBitOr();

    ExprPtr parseBitXor();

    ExprPtr parseBitAnd();

    ExprPtr parseBitShift();

    ExprPtr parseTerm();

    ExprPtr parseFactor();

    ExprPtr parseUnary();

    ExprPtr parsePower();

    ExprPtr parseCall();

    ExprPtr parsePrimary();

    ExprPtr finishCall(const ExprPtr &expr);

    StmtPtr parseIfStmt();

    StmtPtr parseWhileStmt();

    StmtPtr parseForStmt();

    StmtPtr parseWhenStmt();

    StmtPtr parseContinueStmt();

    StmtPtr parseBreakStmt();

    StmtPtr parseReturnStmt();

    StmtPtr parseEnumStmt();

    std::shared_ptr<std::vector<StmtPtr>> parseBlock();

    StmtPtr parseExpressionStmt();

    StmtPtr parseStatement();

    StmtPtr parseLetDeclaration();

    StmtPtr parseVarDeclaration();

    FunctionStmtPtr parseFunction(const std::string &kind);

    StmtPtr parseClass();

    StmtPtr parseImport();

    StmtPtr parseDeclaration();

    void synchronize();
};