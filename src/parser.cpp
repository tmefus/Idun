#include <iostream>
#include "parser.hpp"


ExprPtr Parser::parseExpression() {
    return parseAssignment();
}

ExprPtr Parser::parseAssignment() {
    auto expr = parseOr();
    if (match(TokenType::EQUAL)) {
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            return std::make_shared<AssignExpr>(var->name_, parseAssignment());
        } else if (auto get = std::dynamic_pointer_cast<GetExpr>(expr)) {
            // someObject.someProperty = value;
            return std::make_shared<SetExpr>(get->expr_, get->name_, parseAssignment());
        }
        error(previous(), "Invalid assignment target.");
    }
    if (match(TokenType::PLUS_EQUAL)) {
        auto equals = previous();
        auto value = parseAssignment();
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            equals->type = TokenType::PLUS;
            equals->lexeme = "+";
            value = std::make_shared<BinaryExpr>(var, equals, value);
            return std::make_shared<AssignExpr>(var->name_, value);
        }
        error(equals, "Invalid assignment target.");
    }
    if (match(TokenType::MINUS_EQUAL)) {
        auto equals = previous();
        auto value = parseAssignment();
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            equals->type = TokenType::MINUS;
            equals->lexeme = "-";
            value = std::make_shared<BinaryExpr>(var, equals, value);
            return std::make_shared<AssignExpr>(var->name_, value);
        }
        error(equals, "Invalid assignment target.");
    }
    if (match(TokenType::STAR_EQUAL)) {
        auto equals = previous();
        auto value = parseAssignment();
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            equals->type = TokenType::STAR;
            equals->lexeme = "*";
            value = std::make_shared<BinaryExpr>(var, equals, value);
            return std::make_shared<AssignExpr>(var->name_, value);
        }
        error(equals, "Invalid assignment target.");
    }
    if (match(TokenType::SLASH_EQUAL)) {
        auto equals = previous();
        auto value = parseAssignment();
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            equals->type = TokenType::SLASH;
            equals->lexeme = "/";
            value = std::make_shared<BinaryExpr>(var, equals, value);
            return std::make_shared<AssignExpr>(var->name_, value);
        }
        error(equals, "Invalid assignment target.");
    }
    if (match(TokenType::MOD_EQUAL)) {
        auto equals = previous();
        auto value = parseAssignment();
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            equals->type = TokenType::MOD;
            equals->lexeme = "%";
            value = std::make_shared<BinaryExpr>(var, equals, value);
            return std::make_shared<AssignExpr>(var->name_, value);
        }
        error(equals, "Invalid assignment target.");
    }
    return expr;
}

ExprPtr Parser::parseOr() {
    auto expr = parseAnd();
    while (match(TokenType::OR)) {
        expr = std::make_shared<LogicalExpr>(expr, previous(), parseAnd());
    }
    return expr;
}

ExprPtr Parser::parseAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AND)) {
        expr = std::make_shared<LogicalExpr>(expr, previous(), parseEquality());
    }
    return expr;
}

ExprPtr Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::NOT_EQUAL, TokenType::EQUAL_EQUAL)) {
        auto p = previous();
        auto e = parseComparison();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseComparison() {
    auto expr = parseInIsExpr();
    while (match(TokenType::LESS, TokenType::LESS_EQUAL, TokenType::GREATER, TokenType::GREATER_EQUAL)) {
        auto p = previous();
        auto e = parseInIsExpr();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

// 是否需要左边的表达式(when语句中不需要),默认需要
ExprPtr Parser::parseInIsExpr(bool needLeft) {
    ExprPtr expr = needLeft ? parseRangeExpr() : nullptr;
    if (check(TokenType::NOT) and not atEnd()) {
        auto next = tokens[current + 1];
        if (next->type == TokenType::IN or next->type == TokenType::IS) {
            auto p = advance(); // NOT
            p->type = check(TokenType::IN) ? TokenType::NOTIN : TokenType::NOTIS;
            p->lexeme = check(TokenType::IN) ? "not in" : "not is";
            advance();  // consume IN or IS
            auto e = parseRangeExpr();
            return std::make_shared<BinaryExpr>(expr, p, e);
        }
    }
    if (match(TokenType::IN, TokenType::IS)) {
        auto p = previous();
        auto e = parseRangeExpr();
        return std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseRangeExpr() {
    auto expr = parseBitOr();
    if (match(TokenType::RANGE)) {
        auto p = previous();
        auto e = parseBitOr();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseBitOr() {
    auto expr = parseBitXor();
    while (match(TokenType::BIT_OR)) {
        auto p = previous();
        auto e = parseBitXor();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseBitXor() {
    auto expr = parseBitAnd();
    while (match(TokenType::BIT_XOR)) {
        auto p = previous();
        auto e = parseBitAnd();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseBitAnd() {
    auto expr = parseBitShift();
    while (match(TokenType::BIT_AND)) {
        auto p = previous();
        auto e = parseBitShift();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseBitShift() {
    auto expr = parseTerm();
    while (match(TokenType::SHIFT_L, TokenType::SHIFT_R, TokenType::SHIFT_RA)) {
        auto p = previous();
        auto e = parseTerm();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::MINUS, TokenType::PLUS)) {
        auto p = previous();
        auto e = parseFactor();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseFactor() {
    auto expr = parseUnary();
    while (match(TokenType::STAR, TokenType::SLASH, TokenType::MOD)) {
        auto p = previous();
        auto e = parseUnary();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseUnary() {
    if (match(TokenType::MINUS, TokenType::NOT, TokenType::BIT_NOT)) {
        auto p = previous();
        auto e = parseUnary();
        return std::make_shared<UnaryExpr>(p, e);
    }
    return parsePower();
}

ExprPtr Parser::parsePower() {
    auto expr = parseCall();
    while (match(TokenType::POWER)) {
        auto p = previous();
        auto e = parseUnary();
        expr = std::make_shared<BinaryExpr>(expr, p, e);
    }
    return expr;
}

ExprPtr Parser::parseCall() {
    auto expr = parsePrimary();
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(expr);
        } else if (match(TokenType::DOT)) {
            auto name = consume(TokenType::IDENTIFIER, "Expect property name after '.'");
            expr = std::make_shared<GetExpr>(expr, name);
        } else {
            break;
        }
    }
    return expr;
}

ExprPtr Parser::finishCall(const ExprPtr &expr) {
    auto args = std::make_shared<std::vector<ExprPtr>>();
    if (not check(TokenType::RIGHT_PAREN)) {
        do {
            if (args->size() >= 255) error(peek(), "Can't have more than 255 arguments");
            args->push_back(parseOr());
        } while (match(TokenType::COMMA));
    }
    auto paren = consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");
    return std::make_shared<CallExpr>(expr, paren, args);
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::INTEGER, TokenType::FLOATING)) {
        return std::make_shared<LiteralExpr>(previous()->value);
    }
    if (match(TokenType::IDENTIFIER)) {
        return std::make_shared<VariableExpr>(previous());
    }
    if (match(TokenType::STR_START)) {
        auto strs = std::make_shared<std::vector<ExprPtr>>();
        while (true) {
            if (match(TokenType::STRING)) {
                strs->emplace_back(std::make_shared<LiteralExpr>(previous()->value));
            } else if (match(TokenType::STR_END)) {
                break;
            } else {
                strs->emplace_back(parseOr());
            }
        }
        return std::make_shared<StrExpr>(strs);
    }
    if (match(TokenType::TRUE, TokenType::FALSE)) {
        return std::make_shared<LiteralExpr>(std::make_shared<LoxBool>(previous()->type == TokenType::TRUE));
    }
    if (match(TokenType::LEFT_PAREN)) {
        auto exp = parseOr();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return std::make_shared<GroupingExpr>(exp);
    }
    if (match(TokenType::NIL)) {
        return std::make_shared<LiteralExpr>(std::make_shared<LoxNil>());
    }
    if (match(TokenType::THIS)) {
        return std::make_shared<ThisExpr>(previous());
    }
    if (match(TokenType::SUPER)) {
        auto keyword = previous();
        consume(TokenType::DOT, "Expect '.' after 'super'.");
        auto method = consume(TokenType::IDENTIFIER, "Expect superclass method name.");
        return std::make_shared<SuperExpr>(keyword, method);
    }
    throw error(peek(), "Expected expression");
}

StmtPtr Parser::parseIfStmt() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after if");
    auto cond = parseOr();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    auto then = parseStatement();

    auto firstIfStmt = std::make_shared<IfStmt>(cond, then, nullptr);
    IfStmtPtr lastElse = nullptr;
    while (match(TokenType::ELIF)) {
        consume(TokenType::LEFT_PAREN, "Expected '(' after elif");
        auto elifCond = parseOr();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after elif condition");
        auto elifThen = parseStatement();

        auto newElifBlock = std::make_shared<IfStmt>(elifCond, elifThen, nullptr);
        if (lastElse == nullptr) {
            firstIfStmt->elseStmt_ = newElifBlock;
        } else {
            lastElse->elseStmt_ = newElifBlock;
        }
        lastElse = newElifBlock;
    }
    if (match(TokenType::ELSE)) {
        (lastElse ? lastElse->elseStmt_ : firstIfStmt->elseStmt_) = parseStatement();
    }
    return firstIfStmt;
}

StmtPtr Parser::parseWhileStmt() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after while");
    auto cond = parseOr();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    auto whileBlock = parseStatement();
    return std::make_shared<WhileStmt>(cond, whileBlock);
}

StmtPtr Parser::parseForStmt() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after for");
    auto variable = consume(TokenType::IDENTIFIER, "Expected class name.");
    consume(TokenType::IN, "Expected 'in' after variable");
    auto iterable = parseRangeExpr();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after  iterable");
    auto body = parseStatement();
    return std::make_shared<ForStmt>(variable, iterable, body);
}

StmtPtr Parser::parseWhenStmt() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after when");
    auto whenCond = parseOr();  // 条件的一半
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    consume(TokenType::LEFT_BRACE, "Expected '{' after ')'");

    auto branches = std::make_shared<std::vector<std::pair<std::vector<ExprPtr>, StmtPtr>>>();
    do {
        auto conds = std::vector<ExprPtr>();
        do {
            auto cond = parseInIsExpr(false);
            if (cond == nullptr) error(previous(), "Condition error.");
            if (auto right = std::dynamic_pointer_cast<BinaryExpr>(cond)) {
                if (right->op_->type == TokenType::IN or right->op_->type == TokenType::IS
                    or right->op_->type == TokenType::NOTIN or right->op_->type == TokenType::NOTIS) {
                    right->left_ = whenCond;
                    conds.push_back(right);
                } else { // 不是 IN IS 的情况
                    error(right->op_, "Unsupported condition.");
                }
            } else {    // 单个条件
                auto eq
                        = std::make_shared<Token>(TokenType::EQUAL_EQUAL, nullptr, "==", previous()->line);
                conds.emplace_back(std::make_shared<BinaryExpr>(whenCond, eq, cond));
            }
        } while (match(TokenType::COMMA));
        consume(TokenType::ARROW, "Expected '->' after cond");
        auto block = parseStatement();
        branches->emplace_back(conds, block);
    } while (not check(TokenType::ELSE) and not atEnd());
    consume(TokenType::ELSE, "Expected 'else' in branck last");
    consume(TokenType::ARROW, "Expected '->' after 'else'");
    auto elseBlock = parseStatement();
    consume(TokenType::RIGHT_BRACE, "Expected '}' end of when");
    return std::make_shared<WhenStmt>(branches, elseBlock);
}

StmtPtr Parser::parseContinueStmt() {
    auto continueToken = previous();
    consume(TokenType::SEMICOLON, "Expected ';' after continue statement.");
    return std::make_shared<ContinueStmt>(continueToken);
}

StmtPtr Parser::parseBreakStmt() {
    auto continueToken = previous();
    consume(TokenType::SEMICOLON, "Expected ';' after break statement.");
    return std::make_shared<BreakStmt>(continueToken);
}

StmtPtr Parser::parseReturnStmt() {
    auto returnToken = previous();
    ExprPtr value;
    if (not check(TokenType::SEMICOLON)) {
        value = parseOr();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after return statement.");
    return std::make_shared<ReturnStmt>(returnToken, value);
}

std::shared_ptr<std::vector<StmtPtr>> Parser::parseBlock() {
    auto statements = std::make_shared<std::vector<StmtPtr>>();
    while (not check(TokenType::RIGHT_BRACE) and not atEnd()) {
        statements->push_back(parseDeclaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after block");
    return statements;
}

// todo: 解析import语句
StmtPtr Parser::parseImport() {
    return nullptr;
}

StmtPtr Parser::parseExpressionStmt() {
    auto exp = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<ExpressionStmt>(exp);
}

StmtPtr Parser::parseStatement() {
    if (match(TokenType::IF)) {
        return parseIfStmt();
    }
    if (match(TokenType::FOR)) {
        return parseForStmt();
    }
    if (match(TokenType::WHILE)) {
        return parseWhileStmt();
    }
    if (match(TokenType::RETURN)) {
        return parseReturnStmt();
    }
    if (match(TokenType::WHEN)) {
        return parseWhenStmt();
    }
    if (match(TokenType::LEFT_BRACE)) { // 语句块
        return std::make_shared<BlockStmt>(parseBlock());
    }
    if (match(TokenType::BREAK)) {
        return parseBreakStmt();
    }
    if (match(TokenType::CONTINUE)) {
        return parseContinueStmt();
    }
    if (match(TokenType::IMPORT)) {
        return parseImport();
    }
    // 解析表达式
    return parseExpressionStmt();
}

StmtPtr Parser::parseLetDeclaration() {
    auto identifier = consume(TokenType::IDENTIFIER, "Expected let name.");
    consume(TokenType::EQUAL, "'" + identifier->lexeme + "' must be initialized.");
    ExprPtr init = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after let declaration");
    return std::make_shared<LetStmt>(identifier, init);
}

StmtPtr Parser::parseVarDeclaration() {
    auto identifier = consume(TokenType::IDENTIFIER, "Expected variable name.");
    ExprPtr init = nullptr;
    if (match(TokenType::EQUAL)) {
        init = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after var declaration");
    return std::make_shared<VarStmt>(identifier, init);
}

FunctionStmtPtr Parser::parseFunction(const std::string &kind) {
    auto name = consume(TokenType::IDENTIFIER, "Expected " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expected '(' after " + kind + " name.");
    auto params = std::make_shared<std::vector<TokenPtr>>();
    if (not check(TokenType::RIGHT_PAREN)) {
        do {
            if (params->size() >= 255) error(peek(), "Can't have more than 255 parameters.");
            params->push_back(consume(TokenType::IDENTIFIER, "Expected parameter name."));
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expected '{' before " + kind + " body.");
    auto body = parseBlock();
    return std::make_shared<FunctionStmt>(name, params, body);
}

StmtPtr Parser::parseClass() {
    auto name = consume(TokenType::IDENTIFIER, "Expected class name.");
    VariableExprPtr superClass = nullptr;
    if (match(TokenType::COLON)) {
        consume(TokenType::IDENTIFIER, "Expect superclass name.");
        superClass = std::make_shared<VariableExpr>(previous());
    }
    consume(TokenType::LEFT_BRACE, "Expected '{' before class body.");
    auto methods = std::make_shared<std::vector<FunctionStmtPtr>>();
    while (not check(TokenType::RIGHT_BRACE) or not atEnd()) {
        consume(TokenType::FUN, "Expected the 'fun' keyword in the class body.");
        methods->push_back(parseFunction("method"));
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after class body.");
    return std::make_shared<ClassStmt>(name, superClass, methods);
}

StmtPtr Parser::parseEnumStmt() {
    // todo::
    return nullptr;
}

/* 解析语句的声明
 *
 * */
StmtPtr Parser::parseDeclaration() {
    try {
        if (match(TokenType::VAR)) {
            return parseVarDeclaration();
        }
        if (match(TokenType::LET)) {
            return parseLetDeclaration();
        }
        if (match(TokenType::FUN)) {
            return parseFunction("function");
        }
        if (match(TokenType::CLASS)) {
            return parseClass();
        }
        if (match(TokenType::ENUM)) {
            return parseEnumStmt();
        }
        // 解析其他语句
        return parseStatement();
    } catch (parsing_error &error) {
        parsing_failed = true;
        synchronize();
        return nullptr;
    }
}

void Parser::synchronize() {
    advance();
    while (not atEnd()) {
        if (previous()->type == TokenType::SEMICOLON) return;
        switch (peek()->type) {
            default:break;
            case TokenType::IF:
            case TokenType::FUN:
            case TokenType::LET:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::ENUM:
            case TokenType::BREAK:
            case TokenType::CLASS:
            case TokenType::WHILE:
            case TokenType::IMPORT:
            case TokenType::RETURN:
            case TokenType::CONTINUE: return;
        }
        advance();
    }
}

std::optional<std::vector<StmtPtr>> Parser::parse() {
    std::vector<StmtPtr> statements;
    while (not atEnd()) {
        statements.push_back(parseDeclaration());
    }

    if (parsing_failed) return std::nullopt;

    return statements;
}
