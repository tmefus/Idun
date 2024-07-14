#include <iostream>
#include "resolver.hpp"
#include "interpreter.hpp"


void Resolver::visitAssignExpr(AssignExprPtr expr) {
    // todo: 在这里可以处理let变量的重新赋值问题
    resolve(expr->value_);
    resolveLocal(expr, expr->name_);
}

void Resolver::visitBinaryExpr(BinaryExprPtr expr) {
    resolve(expr->left_);
    resolve(expr->right_);
}

void Resolver::visitGroupingExpr(GroupingExprPtr expr) {
    resolve(expr->expression_);
}

void Resolver::visitLiteralExpr(LiteralExprPtr expr) {}

void Resolver::visitStrExpr(StrExprPtr expr) {
    for (const auto &str: *expr->strs) {
        resolve(str);
    }
}

void Resolver::visitUnaryExpr(UnaryExprPtr expr) {
    resolve(expr->right_);
}

void Resolver::visitVariableExpr(VariableExprPtr expr) {
    if (!scopes.empty()) {
        auto it = scopes.back().find(expr->name_->lexeme);
        // 检查变量只声明未赋值
        if (it != scopes.back().end() && !it->second) {
            std::cerr << "Line [" << expr->name_->line << "]: Can't read local variable in its own initializer.\n";
            has_error_ = true;
        }
    }
    resolveLocal(expr, expr->name_);
}

void Resolver::visitLogicalExpr(LogicalExprPtr expr) {
    resolve(expr->left_);
    resolve(expr->right_);
}

void Resolver::visitCallExpr(CallExprPtr expr) {
    resolve(expr->callee_);
    for (const auto &arg: *expr->args_) {
        resolve(arg);
    }
}

void Resolver::visitGetExpr(GetExprPtr expr) {
    resolve(expr->expr_);
}

void Resolver::visitSetExpr(SetExprPtr expr) {
    resolve(expr->value_);
    resolve(expr->expr_);
}

void Resolver::visitThisExpr(ThisExprPtr expr) {
    if (currentClass == ClassType::NONE) {
        std::cerr << "Line [" << expr->keyword_->line << "]: Can't use 'this' outside of a class.\n";
        has_error_ = true;
        return;
    }

    resolveLocal(expr, expr->keyword_);
}

void Resolver::visitSuperExpr(SuperExprPtr expr) {
    if (currentClass == ClassType::NONE) {
        std::cerr << "Line [" << expr->keyword_->line << "]: Can't use 'super' outside of a class.\n";
        has_error_ = true;
    } else if (currentClass != ClassType::SUBCLASS) {
        std::cerr << "Line [" << expr->keyword_->line << "]: Can't use 'super' in a class with no superclass.\n";
        has_error_ = true;
    }

    resolveLocal(expr, expr->keyword_);
}

void Resolver::visitIfStmt(IfStmtPtr stmt) {
    resolve(stmt->condition_);
    resolve(stmt->thenStmt_);
    if (stmt->elseStmt_) resolve(stmt->elseStmt_);
}

void Resolver::visitWhileStmt(WhileStmtPtr stmt) {
    // 处理是否在循环之中
    auto previousType = currentBlock;
    currentBlock = BlockType::LOOP;

    resolve(stmt->condition_);
    resolve(stmt->statements_);

    currentBlock = previousType;
}

void Resolver::visitContinueStmt(ContinueStmtPtr stmt) {
    if (currentBlock != BlockType::LOOP) {
        std::cerr << "Line [" << stmt->keyword_->line << "]: 'continue' can only be used in loops." << std::endl;
        has_error_ = true;
    }
}

void Resolver::visitBreakStmt(BreakStmtPtr stmt) {
    if (currentBlock != BlockType::LOOP) {
        std::cerr << "Line [" << stmt->keyword_->line << "]: 'break' can only be used in loops." << std::endl;
        has_error_ = true;
    }
}

void Resolver::visitForStmt(ForStmtPtr stmt) {
    declare(stmt->variable_);
    resolve(stmt->iterable_);
    resolve(stmt->body_);
}

void Resolver::visitWhenStmt(WhenStmtPtr stmt) {
    // when的所有分支
    for (const auto &conds_block: *stmt->branches) {
        // 某个分支的所有条件
        for (const auto &cond: conds_block.first) {
            resolve(cond);
        }
        resolve(conds_block.second); // 某个分支的语句块
    }
    resolve(stmt->else_);  // 最后的else分支
}

void Resolver::visitBlockStmt(BlockStmtPtr stmt) {
    beginScope();
    resolve(stmt->statements_);
    endScope();
}

void Resolver::visitExpressionStmt(ExpressionStmtPtr stmt) {
    resolve(stmt->expression_);
}

void Resolver::visitLetStmt(LetStmtPtr stmt) {
    declare(stmt->name_);
    resolve(stmt->initializer_);
    define(stmt->name_);
}

void Resolver::visitVarStmt(VarStmtPtr stmt) {
    declare(stmt->name_);
    if (stmt->initializer_) {
        resolve(stmt->initializer_);
    }
    define(stmt->name_);
}

void Resolver::visitFunctionStmt(FunctionStmtPtr stmt) {
    declare(stmt->name_);
    define(stmt->name_);
    resolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::visitReturnStmt(ReturnStmtPtr stmt) {
    if (currentFunction == FunctionType::NONE) {
        std::cerr << "Line [" << stmt->keyword_->line << "]: Can't return from top-level code." << std::endl;
        has_error_ = true;
    }
    if (stmt->value_) {
        if (currentFunction == FunctionType::INITIALIZER) {
            std::cerr << "Line [" << stmt->keyword_->line << "]: Can't return a value from an initializer." << std::endl;
            has_error_ = true;
        }
        resolve(stmt->value_);
    }
}

void Resolver::visitClassStmt(ClassStmtPtr stmt) {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;

    declare(stmt->name_);
    define(stmt->name_);

    if (stmt->superClass_) {
        if (stmt->name_->lexeme == stmt->superClass_->name_->lexeme) {
            std::cerr << "Line [" << stmt->superClass_->name_->line << "]: A class can't inherit from itself." << std::endl;
            has_error_ = true;
        }
        currentClass = ClassType::SUBCLASS;
        resolve(stmt->superClass_);
    }

    if (stmt->superClass_) {
        beginScope();
        scopes.back().insert_or_assign("super", true);
    }

    beginScope();
    scopes.back().insert_or_assign("this", true);

    for (const auto &method: *stmt->methods_) {
        auto declaration = FunctionType::METHOD;
        if (method->name_->lexeme == "init") {
            declaration = FunctionType::INITIALIZER;
        }
        resolveFunction(method, declaration);
    }

    endScope();

    if (stmt->superClass_) endScope(); // 对应 super

    currentClass = enclosingClass;
}

void Resolver::resolve(const std::shared_ptr<std::vector<StmtPtr>> &stmts) {
    for (const auto &stmt: *stmts) {
        resolve(stmt);
    }
}

void Resolver::resolve(const StmtPtr &stmt) {
    stmt->accept(*this);
}

void Resolver::resolve(const ExprPtr &expr) {
    expr->accept(*this);
}

void Resolver::resolveLocal(const ExprPtr &expr, const TokenPtr &name) {
    for (int i = (int) scopes.size() - 1; i >= 0; --i) {
        if (scopes.at(i).contains(name->lexeme)) {
            // depth参数表示以当前作用域为0，父作用域依次+1
            interpreter.resolve(expr, (int) scopes.size() - i - 1);
            return;
        }
    }
}

void Resolver::resolveFunction(const FunctionStmtPtr &stmt, FunctionType type) {
    // 处理是否在函数声明
    auto previousType = currentFunction;
    currentFunction = type;

    beginScope();

    for (const auto &param: *stmt->params_) {
        declare(param);
        define(param);
    }
    resolve(stmt->body_);

    endScope();

    currentFunction = previousType;
}

void Resolver::beginScope() {
    scopes.emplace_back();
}

void Resolver::endScope() {
    scopes.pop_back();
}

// 声明变量
void Resolver::declare(const TokenPtr &name) {
    if (scopes.empty()) return;
    auto &scope = scopes.back();
    if (scope.contains(name->lexeme)) {
        std::cerr << "Line [" << name->line << "]: Already a variable with this name in this scope" << std::endl;
        has_error_ = true;
    }
    scope.insert({name->lexeme, false});
}

// 变量定义
void Resolver::define(const TokenPtr &name) {
    if (scopes.empty()) return;
    scopes.back().insert_or_assign(name->lexeme, true);
}

bool Resolver::resolve(const std::vector<StmtPtr> &ast) {
    for (const auto &stmt: ast) {
        resolve(stmt);
    }
    return !has_error_;
}